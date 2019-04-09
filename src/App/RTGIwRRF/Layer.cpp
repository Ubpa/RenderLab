#include "Layer.h"

#include "Model.h"
#include "Unit.h"

#include <CppUtil/Basic/Error.h>

#include <sstream>

using namespace App;
using namespace CppUtil::Basic;
using namespace std;

void Layer::Init() {
	if (model.expired())
		return;

	model.lock()->AddLayer(This<Layer>());
}

bool Layer::SetModel(PtrC<Model> model) const {
	static constexpr bool ERROR = false;

	if (!this->model.expired())
		return ERROR;

	this->model = model;
	model->AddLayer(This<Layer>());
	return true;
}

int Layer::GetIDof(PtrC<Unit> unit) const {
	static constexpr int ERROR = -1;
	
	if (unit == nullptr)
		return ERROR;

	for (int i = 0; i < units.size(); i++) {
		if (units[i] == unit)
			return i;
	}

	return ERROR;
}

int Layer::GetID() const {
	static constexpr int ERROR = -1;
	
	if (model.expired())
		return ERROR;

	return model.lock()->GetIDof(This<Layer>());
}

const string Layer::GetFuncName() const {
	static auto const & ERROR = ErrorRetVal(&Layer::GetFuncName);

	if (model.expired())
		return ERROR;

	stringstream name;
	name << model.lock()->GetFuncName();
	name << "_Layer_" << GetID() << "_" << connection._to_string() << "_" << activation._to_string();
	return name.str();
}

bool Layer::AddUnit(PtrC<Unit> unit) const {
	static auto const & ERROR = false;

	if (unit == nullptr) {
		printf("ERROR: unit ptr is nullptr\n");
		return ERROR;
	}

	if (unit->GetLayer() != nullptr && unit->GetLayer() != This()) {
		printf("ERROR: unit's layer is not this layer\n");
		return ERROR;
	}

	switch (connection)
	{
	case Connection::Dense: {
		if (unit->GetInputDim() != inputDim) {
			printf("ERROR: unit's input dim is not %d\n", inputDim);
			return ERROR;
		}

		unit->SetLayer(This<Layer>());
		units.push_back(unit);

		break;
	}
	default: {
		printf("ERROR: not support connection [%s]\n", connection._to_string());
		return ERROR;
	}
	}

	return true;
}

const string Layer::GenFunc(bool genUnits) const {
	static auto const & ERROR = ErrorRetVal(&Layer::GenFunc);

	if (!IsValid()) {
		printf("ERROR: layer is not valid\n");
		return ERROR;
	}

	const string indent = "    ";

	stringstream rst;

	// layer func declaration
	rst << "void " << GetFuncName() << "(";
	for (int i = 0; i < GetInputDim(); i++)
		rst << "float x" << i << ",";
	rst << endl << indent;
	for (int i = 0; i < GetOutputDim(); i++) {
		rst << "out float h" << i;
		if (i < GetOutputDim() - 1)
			rst << ",";
	}
	rst << ")" << endl;

	// layer func definition

	rst << "{" << endl;

	switch (connection)
	{
	case Connection::Dense: {
		// arg list
		stringstream inArgList;
		inArgList << "(";
		for (int i = 0; i < GetInputDim(); i++) {
			inArgList << "x" << i;
			if (i < GetInputDim() - 1)
				inArgList << ", ";
		}
		inArgList << ")";

		// compute h
		for (int i = 0; i < GetOutputDim(); i++) {
			rst << indent << "h" << i << " = "
				<< units[i]->GenComputeExpr() << "; "
				<< endl;
		}

		break;
	}
	default: {
		printf("ERROR: connection [%s] is not supported\n", connection._to_string());
		return ERROR;
	}
	}

	rst << "}" << endl;

	return rst.str();
}

bool Layer::IsValid() const {
	static constexpr bool ERROR = false;

	if (model.expired()
		|| units.size() == 0
		|| inputDim == -1)
	{
		return ERROR;
	}

	return true;
}
