#include "Picker.h"

#include <UI/Attribute.h>

#include <CppUtil/Engine/MeshEdit/DeformRBF.h>
#include <CppUtil/Engine/Viewer.h>
#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/RayIntersector.h>
#include <CppUtil/Engine/Ray.h>
#include <CppUtil/Engine/Roamer.h>
#include <CppUtil/Engine/CmptGeometry.h>
#include <CppUtil/Engine/TriMesh.h>

#include <CppUtil/Qt/RawAPI_OGLW.h>

#include <CppUtil/OpenGL/Camera.h>

#include <CppUtil/Basic/LambdaOp.h>
#include <CppUtil/Basic/EventManager.h>

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
		Point3 posOnScreen(x / float(pOGWL->w), 1 - y / float(pOGWL->h), 1);
		Point3 posInNorm = Vec3(posOnScreen) * 2.0f - Vec3(1.0f);
		Point3 posInWorld = (camera->GetProjectionMatrix() * camera->GetViewMatrix()).Inverse()(posInNorm);

		Vec3 dir = (posInWorld - camera->GetPos()).Normalize();
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
				this->dir = triMesh->GetNormals()[idx];
				printf("hit triMesh, idx:%d, pos: (%f,%f,%f)\n", closestRst.idx, pos.x, pos.y, pos.z);
				
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

						if (Point3::Distance(pos, positions[u]) < r)
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
		if (isInDeform && Math::Abs(lastY-pOGWL->y)>2) {
			auto con = cons.back();
			cons.push_back({con.id, origPos + 0.04f * (y - pOGWL->y) * dir });
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
