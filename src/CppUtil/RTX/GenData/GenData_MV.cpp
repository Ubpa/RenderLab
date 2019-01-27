#include "GenData_MV.h"
#include "GenData_TV.h"

#include <CppUtil/RTX/TexVisitor.h>
#include <CppUtil/RTX/Lambertian.h>
#include <CppUtil/RTX/Metal.h>
#include <CppUtil/RTX/Dielectric.h>
#include <CppUtil/RTX/Light.h>
#include <CppUtil/RTX/Isotropic.h>

#include <CppUtil/RTX/Texture.h>

using namespace RTX;
using namespace std;

// MatT : Material Type
const float MatT_Lambertian = 0.0f;
const float MatT_Metal      = 1.0f;
const float MatT_Dielectric = 2.0f;
const float MatT_Light      = 3.0f;
const float MatT_Isotropic  = 4.0f;

void GenData_MV::Visit(Lambertian::CPtr lambertian) {
	if (lambertian == NULL)
		return;

	auto targetPair = mat2idx.find(lambertian);
	if (targetPair != mat2idx.end())
		return;

	mat2idx[lambertian] = matData.size();

	matData.push_back(MatT_Lambertian);

	if (lambertian->GetTexture())
		tex2idxVec[lambertian->GetTexture()].push_back(matData.size());
	matData.push_back(-1);
}

void GenData_MV::Visit(Metal::CPtr metal) {
	if (metal == NULL)
		return;

	auto targetPair = mat2idx.find(metal);
	if (targetPair != mat2idx.end())
		return;

	mat2idx[metal] = matData.size();

	matData.push_back(MatT_Metal);

	if(metal->GetTexture())
		tex2idxVec[metal->GetTexture()].push_back(matData.size());
	matData.push_back(-1);

	matData.push_back(metal->GetFuzz());
}

void GenData_MV::Visit(Dielectric::CPtr dielectric) {
	if (dielectric == NULL)
		return;

	auto targetPair = mat2idx.find(dielectric);
	if (targetPair != mat2idx.end())
		return;

	mat2idx[dielectric] = matData.size();

	matData.push_back(MatT_Dielectric);

	matData.push_back(dielectric->GetRafractIndex());
}

void GenData_MV::Visit(Light::CPtr light) {
	if (light == NULL)
		return;

	auto targetPair = mat2idx.find(light);
	if (targetPair != mat2idx.end())
		return;

	mat2idx[light] = matData.size();

	matData.push_back(MatT_Light);
	if (light->GetTexture())
		tex2idxVec[light->GetTexture()].push_back(matData.size());
	matData.push_back(-1);

	matData.push_back(light->GetLinear());
	matData.push_back(light->GetQuadratic());
}

void GenData_MV::Visit(Isotropic::CPtr isotropic) {
	if (isotropic == NULL)
		return;

	auto targetPair = mat2idx.find(isotropic);
	if (targetPair != mat2idx.end())
		return;

	mat2idx[isotropic] = matData.size();

	matData.push_back(MatT_Isotropic);

	if (isotropic->GetTexture())
		tex2idxVec[isotropic->GetTexture()].push_back(matData.size());
	matData.push_back(-1);
}

void GenData_MV::SetTex(const TexIdxMap & tex2idx) {
	for (auto & pair : tex2idx) {
		auto target = tex2idxVec.find(pair.first);
		if (target != tex2idxVec.end()) {
			for (auto const & idx : target->second)
				matData[idx] = pair.second;
		}
	}
}

void GenData_MV::Accept(GenData_TV::Ptr genFS_TV) {
	for (auto const & pair : tex2idxVec)
		pair.first->Accept(genFS_TV);

	SetTex(genFS_TV->GetTexIdxMap());
}