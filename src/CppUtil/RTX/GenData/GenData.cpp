#include <CppUtil/RTX/GenData.h>

#include "GenData_HV.h"
#include "GenData_MV.h"
#include "GenData_TV.h"

#include <CppUtil/RTX/Group.h>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace std;

GenData::GenData(CppUtil::Basic::CPtr<Hitable> scene)
	: hitableVisitor(ToPtr(new GenData_HV(packData))),
	matVisitor(ToPtr(new GenData_MV)),
	texVisitor(ToPtr(new GenData_TV(packData)))
{
	auto group = new Group;
	group->push_back(scene);
	root = ToCPtr(group);

	root->Accept(hitableVisitor);
	hitableVisitor->Accept(matVisitor);
	matVisitor->Accept(texVisitor);
}


const vector<float> GenData::GetSceneData() const {
	return hitableVisitor->GetSceneData();
}

const vector<float> GenData::GetMatData() const {
	return matVisitor->GetMatData();
}

const vector<float> GenData::GetTexData() const {
	return texVisitor->GetTexData();
}

const vector<float> GenData::GetPackData() const {
	return packData;
}

const GenData::ImgIdxMap GenData::GetImgIdxMap() const {
	return texVisitor->GetImgIdxMap();
}

const GenData::SkyboxImgs & GenData::GetSkyboxImgs() const {
	return texVisitor->GetSkyboxImgs();
}