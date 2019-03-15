#include "Unit.h"

#include "Layer.h"

#include <CppUtil/Basic/Error.h>

#include <sstream>

using namespace App;
using namespace CppUtil::Basic;
using namespace std;

void Unit::InitAfterGenSharePtr() {
	if (layer.expired())
		return;

	layer.lock()->AddUnit(CThis());
}

bool Unit::SetLayer(Layer::CPtr layer) const {
	static constexpr bool ERROR = false;

	if (!this->layer.expired())
		return ERROR;

	this->layer = layer;
	layer->AddUnit(CThis());
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

	return layer.lock()->GetIDof(CThis());
}

const string Unit::GenFunc() const {
	static auto const & ERROR = ErrorRetVal(&Unit::GenFunc);

	if (!IsValid())
	{
		printf("ERROR: unit is not valid\n");
		return ERROR;
	}

	stringstream unitFunc;
	unitFunc << "float " << GetFuncName();

	switch (layer.lock()->GetConnection())
	{
	case Connection::Dense: {
		// args list
		unitFunc << endl << "(" << endl;
		for (int i = 0; i < weights.size() - 1; i++) {
			unitFunc << "    float x" << i;
			if (i < weights.size() - 2)
				unitFunc << "," << endl;
			else
				unitFunc << endl << ")" << endl;
		}

		// define
		unitFunc << "{" << endl;

		// z = dot(weights, [x,1]);
		unitFunc << "    float z =" << endl;
		for (int i = 0; i < weights.size(); i++) {
			unitFunc << "        ";
			if (i == 0)
				unitFunc << "  ";
			else
				unitFunc << "+ ";

			if (i < weights.size() - 1)
				unitFunc << "x" << i << " * (" << weights[i] << ")" << endl;
			else
				unitFunc << "(" << weights[i] << ")" << ";" << endl << endl;
		}
		break;
	}
	default: {
		printf("ERROR: connection [%s] not support\n", layer.lock()->GetConnection()._to_string());
		return ERROR;
	}
	}

	// h = activation(z);
	unitFunc << "    float h = ";
	switch (layer.lock()->GetActivation())
	{
	case Activation::Identity: {
		unitFunc << "z;// Activation : Indentity" << endl << endl;
		break;
	}
	case Activation::ReLU: {
		unitFunc << "max(0, z);// Activation : ReLU" << endl << endl;
		break;
	}
	default: {
		printf("ERROR: Activation is not supported.\n");
		return ERROR;
		break;
	}
	}

	unitFunc << "    return h;" << endl;
	unitFunc << "}" << endl;

	return unitFunc.str();
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
