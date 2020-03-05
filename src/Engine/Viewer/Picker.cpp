#include "Picker.h"

#include <UI/Attribute.h>

#include <Engine/MeshEdit/DeformRBF.h>
#include <Engine/Viewer.h>
#include <Engine/Scene.h>
#include <Engine/SObj.h>
#include <Engine/RayIntersector.h>
#include <Engine/Ray.h>
#include <Engine/Roamer.h>
#include <Engine/CmptGeometry.h>
#include <Engine/TriMesh.h>

#include <Qt/RawAPI_OGLW.h>

#include <OpenGL/Camera.h>

#include <Basic/LambdaOp.h>
#include <Basic/EventManager.h>

#include <QtWidgets/QApplication>

#include <deque>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

Picker::Picker(Viewer * viewer)
	: viewer(viewer), rayIntersector(RayIntersector::New()), deformRBF(DeformRBF::New(nullptr)) { }

void Picker::Init() {
	auto MRB_PressOp = LambdaOp_New([this]() {
		Viewer * viewer = this->GetViewer();
		auto pOGWL = viewer->GetOGLW();
		auto camera = viewer->GetRoamer()->GetCamera();

		int x = pOGWL->x;
		int y = pOGWL->y;

		// y 需要反过来
		Ubpa::pointf3 posOnScreen(2 * x / float(pOGWL->w), 2 * (1 - y / float(pOGWL->h)), 2);
		Ubpa::pointf3 posInNorm = posOnScreen - Ubpa::vecf3(1.0f);
		Ubpa::pointf3 posInWorld = (camera->GetProjectionMatrix() * camera->GetViewMatrix()).inverse() * posInNorm;

		Ubpa::vecf3 dir = (posInWorld - camera->GetPos()).normalize();
		Ray ray(camera->GetPos(), dir);
		rayIntersector->Init(&ray);
		viewer->GetScene()->GetRoot()->Accept(rayIntersector);
		auto closestRst = rayIntersector->GetRst();

		bool canDeform = false;
		GS::GetV("canDeform", canDeform);
		if (canDeform && !isInDeform && closestRst.closestSObj) {
			auto cmptGeo = closestRst.closestSObj->GetComponent<CmptGeometry>();
			auto triMesh = CastTo<TriMesh>(cmptGeo->primitive);
			if (triMesh) {
				isInDeform = true;
				this->y = pOGWL->y;
				const auto & positions = triMesh->GetPositions();
				auto pos = positions[closestRst.idx];
				origPos = pos;
				size_t idx = static_cast<size_t>(closestRst.idx);
				this->dir = triMesh->GetNormals()[idx].cast_to<Ubpa::vecf3>();
				printf("hit triMesh, idx:%d, pos: (%f,%f,%f)\n", closestRst.idx, pos[0], pos[1], pos[2]);
				
				deformRBF = DeformRBF::New(triMesh);

				vector<set<size_t>> adj(triMesh->GetPositions().size());
				for (auto f : triMesh->GetTriangles()) {
					adj[f->idx[0]].insert(f->idx[1]);
					adj[f->idx[0]].insert(f->idx[2]);
					adj[f->idx[1]].insert(f->idx[0]);
					adj[f->idx[1]].insert(f->idx[2]);
					adj[f->idx[2]].insert(f->idx[0]);
					adj[f->idx[2]].insert(f->idx[1]);
				}

				float r = 0.15f;
				deque<size_t> pool;
				pool.push_back(idx);
				set<size_t> inner;
				set<size_t> boundary;
				while(!pool.empty()) {
					auto v = pool.front();
					pool.pop_front();
					inner.insert(v);
					for (auto u : adj[v]) {
						if (inner.find(u) != inner.end())
							continue;

						if (Ubpa::pointf3::distance(pos, positions[u]) < r)
							pool.push_back(u);
						else
							boundary.insert(u);
					}
				}
				cons = vector<DeformRBF::Constraint>();
				for (auto v : boundary)
					cons.push_back({ v, positions[v] });
				cons.push_back({ idx, triMesh->GetPositions()[idx]});
				deformIndice = vector<size_t>({ inner.begin(), inner.end() });
				deformRBF->Run(cons, deformIndice);
				pOGWL->DirtyVAO(triMesh);
			}
		}
		
		Ui::Attribute::GetInstance()->SetSObj(closestRst.closestSObj);
	}, true);
	EventMngr::GetInstance().Reg(Qt::LeftButton, (void*)viewer->GetOGLW(), EventMngr::MOUSE_PRESS, MRB_PressOp);

	auto changeDeform = LambdaOp_New([this]() {
		Viewer * viewer = this->GetViewer();
		auto pOGWL = viewer->GetOGLW();
		if (isInDeform && std::abs(lastY-pOGWL->y)>2) {
			auto con = cons.back();
			DeformRBF::Constraint c = { con.id, origPos + 0.04f * (y - pOGWL->y) * dir };
			cons.push_back(c);
			deformRBF->Run(cons, deformIndice);
			pOGWL->DirtyVAO(deformRBF->GetMesh());
			lastY = pOGWL->y;
		}
	}, true);

	EventMngr::GetInstance().Reg(Qt::NoButton, (void*)viewer->GetOGLW(), EventMngr::MOUSE_MOVE, changeDeform);

	auto outDeform = LambdaOp_New([this]() {
		isInDeform = false;
		deformRBF->Clear();
		cons.clear();
		deformIndice.clear();
	}, true);
	EventMngr::GetInstance().Reg(Qt::LeftButton, (void*)viewer->GetOGLW(), EventMngr::MOUSE_RELEASE, outDeform);
}
