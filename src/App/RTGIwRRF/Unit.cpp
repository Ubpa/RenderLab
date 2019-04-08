#include "Unit.h"

#include "Layer.h"

#include <CppUtil/Basic/Error.h>

#include <sstream>

using namespace App;
using namespace CppUtil::Basic;
using namespace std;

void Unit::Init() {
	if (layer.expired())
		return;

	layer.lock()->AddUnit(This());
}

bool Unit::SetLayer(CPtr<Layer> layer) const {
	static constexpr bool ERROR = false;

	if (!this->layer.expired())
		return ERROR;

	this->layer = layer;
	layer->AddUnit(This());
	return true;
}

const string Unit::GetFuncName() const {
	static auto const & ERROR = ErrorRetVal(&Unit::GetFuncName);
	
	if (layer.expired()) {
		printf("ERROR: layer is expored.\n");
		return ERROR;
	}

	stringstream name;
	name << layer.lock()->GetFuncName();
	name << "_Unit_" << GetID();
	return name.str();
}

int Unit::GetID() const {
	static constexpr int ERROR = -1;

	if (layer.expired()) {
		printf("ERROR: layer is expored.\n");
		return ERROR;
	}

	return layer.lock()->GetIDof(This());
}

const string Unit::GenComputeExpr() const {
	static auto const & ERROR = ErrorRetVal(&Unit::GenComputeExpr);

	if (!IsValid())
	{
		printf("ERROR: unit is not valid\n");
		return ERROR;
	}

	stringstream rst;

	switch (layer.lock()->GetConnection())
	{
	case Connection::Dense: {
		stringstream computeZ;
		for (int i = 0; i < GetInputDim(); i++)
			computeZ << "x" << i << "*(" << weights[i] << ")+";
		computeZ << "(" << weights.back() << ")";

		switch (layer.lock()->GetActivation())
		{
		case Activation::Identity: {
			rst << computeZ.str();
			break;
		}
		case Activation::ReLU: {
			rst << "max(0, "<< computeZ.str() <<")";
			break;
		}
		case Activation::tanh: {
			rst << "tanh(" << computeZ.str() << ")";
			break;
		}
		}
		break;
	}
	default: {
		printf("ERROR: connection [%s] not support\n", layer.lock()->GetConnection()._to_string());
		return ERROR;
	}
	}

	return rst.str();
}

bool Unit::IsValid() const {
	static constexpr bool ERROR = false;

	if (layer.expired()
		|| weights.size() == 0)
	{
		return ERROR;
	}

	return true;
}
