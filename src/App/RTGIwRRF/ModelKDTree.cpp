#include "ModelKDTree.h"

#include "Model.h"

#include <CppUtil/Basic/Error.h>

#include <sstream>

using namespace App;
using namespace CppUtil::Basic;
using namespace std;

const float ModelKDTree::interpolateRatio = 0.2f;

const std::string ModelKDTree::GenFunc(
	const vector<float> & minVal,
	const vector<float> & extent) const
{
	static auto const & ERROR = ErrorRetVal(&ModelKDTree::GenFunc);

	if (!IsValid()) {
		printf("ERROR: Model KDTree is not valid\n");
		return ERROR;
	}

	const string indent = "    ";

	stringstream rst;

	rst << GenFuncRecursion() << endl;

	// func declaration
	rst << "void Model" << "(";
	for (int i = 0; i < inputDim; i++)
		rst << "float x" << i << ",";
	rst << indent << endl;
	for (int i = 0; i < outputDim; i++) {
		rst << "out float h" << i;
		if (i < outputDim - 1)
			rst << ",";
	}
	rst << ")" << endl;

	// func definition
	rst << "{" << endl;

	// map to unit hyperbox
	rst << indent << "// map to unit hyperbox" << endl;
	for (int i = 0; i < inputDim; i++) {
		rst << indent << "x" << i << " = ";
		rst << "(x" << i << " - (" << minVal[i] << ")) / (" << extent[i] << ");" << endl;
	}
	rst << endl;

	// call root
	rst << indent << GetFuncName() << GenCallArgList("x", "h") << endl << endl;

	// map back
	rst << indent << "// map back" << endl;
	for (int i = 0; i < outputDim; i++) {
		rst << indent << "h" << i << " = ";
		rst << "h" << i << " * (" << extent[i] << ") + (" << minVal[i] << ")";
		rst << ";" << endl;
	}

	rst << "}" << endl;

	return rst.str();
}

const string ModelKDTree::GenFuncRecursion() const {
	static const auto & ERROR = ErrorRetVal(&ModelKDTree::GenFuncRecursion);

	if (!IsValid()) {
		printf("ERROR: Model KDTree is not valid\n");
		return ERROR;
	}

	const string indent = "    ";
	const string indent2 = indent + indent;

	stringstream rst;

	if (GetLeft()) {
		auto leftStr = GetLeft()->GenFuncRecursion();
		if (IsError(leftStr))
			return ERROR;

		rst << leftStr << endl;
	}

	if (GetRight()) {
		auto rightStr = GetRight()->GenFuncRecursion();
		if (IsError(rightStr))
			return ERROR;

		rst << rightStr << endl;
	}

	if (GetData()) {
		auto modelStr = GetData()->GenFunc();
		if (IsError(modelStr))
			return ERROR;

		rst << modelStr << endl;
	}

	// func declaration
	rst << "void " << GetFuncName() << "(";
	for (int i = 0; i < inputDim; i++)
		rst << "float x" << i << ",";
	rst << endl << indent;
	for (int i = 0; i < outputDim; i++) {
		rst << "out float h" << i;
		if (i < outputDim - 1)
			rst << ",";
	}
	rst << ")" << endl;

	// func definition
	rst << "{" << endl;

	// kdTree
	if (HasTwoChild()) {
		const float axisExtent = GetSpiltAxisExtent();
		const float spiltVal = GetSpiltVal();

		rst << indent << "float interpolateExtent = " << axisExtent << "* interpolateRatio;" << endl;
		rst << indent << "float delta = interpolateExtent / 2;" << endl;
		rst << indent << "float lowBound = " << spiltVal << " - delta;" << endl;
		rst << indent << "float highBound = " << spiltVal << " + delta;" << endl;

		// left
		rst << indent << "if ( x" << GetAxis() << " < lowBound ) {" << endl;
		rst << indent2 << GetLeft()->GetFuncName() << GenCallArgList("x", "h") << endl;
		rst << indent << "}" << endl;

		// interpolate
		const string leftPrefix = "left_h";
		const string rightPrefix = "right_h";
		rst << indent << "else if ( x" << GetAxis() << " < highBound ) {" << endl;
		// declare output
		for (int i = 0; i < outputDim; i++)
			rst << indent2 << "float " << leftPrefix << i << ";" << endl;
		for (int i = 0; i < outputDim; i++)
			rst << indent2 << "float " << rightPrefix << i << ";" << endl;
		rst << indent2 << endl;
		// evaluate left and right
		rst << indent2 << GetLeft()->GetFuncName() << GenCallArgList("x", leftPrefix) << endl;
		rst << indent2 << GetRight()->GetFuncName() << GenCallArgList("x", rightPrefix) << endl;
		// smootherstep
		rst << indent2 << endl;
		rst << indent2 << "float t = 0.5 + ( x" << GetAxis() << " - " << GetSpiltVal() << " ) / interpolateExtent;" << endl;
		rst << indent2 << "t = smootherstep(t);" << endl;
		for (int i = 0; i < outputDim; i++) {
			rst << indent2 << "h" << i <<
				//" = ( 1 - t ) * " << leftPrefix << i << " + t * " << rightPrefix << i << ";" << endl;
				"= mix(" << leftPrefix << i << "," << rightPrefix << i << ", t);" << endl;
		}
		rst << indent << "}" << endl;

		// right
		rst << indent << "else {" << endl;
		rst << indent2 << GetRight()->GetFuncName() << GenCallArgList("x", "h") << endl;
		rst << indent << "}" << endl;
	}
	else if (IsLeaf())
		rst << indent << GetData()->GetFuncName() << GenCallArgList("x","h") << endl;
	else {
		printf("ERROR: current not support single child");
		return ERROR;
	}

	rst << "}" << endl;

	return rst.str();
}

bool ModelKDTree::IsValid() const {
	static constexpr bool ERROR = false;

	if ((GetLeft() != nullptr && !GetLeft()->IsValid())
		|| (GetRight() != nullptr && !GetRight()->IsValid())
		|| inputDim < 0
		|| outputDim < 0
		|| HasSingleChild()
		|| (HasTwoChild() && (GetData() || GetAxis() < 0 || GetAxis() >= inputDim || GetSpiltVal() < 0 || GetSpiltVal() > 1))
		|| (IsLeaf() && !GetData())
		|| ID < 0
		)
	{
		return ERROR;
	}

	return true;
}

float ModelKDTree::GetSpiltAxisExtent() const {
	float extent = 1.f;
	for (auto ptr = GetParent(); ptr != nullptr; ptr = ptr->GetParent()) {
		if (ptr->GetAxis() == GetAxis())
			extent /= 2;
	}
	return extent;
}

const string ModelKDTree::GetFuncName() const {
	return string("ModelKDTree_") + to_string(ID);
}

const string ModelKDTree::GenCallArgList(const string & xName, const string & hName) const {
	stringstream rst;

	rst << "(";
	for (int i = 0; i < inputDim; i++)
		rst << xName << i << ",";
	for (int i = 0; i < outputDim; i++)
		rst << hName << i << (i != outputDim - 1 ? "," : "");
	rst << ");";

	return rst.str();
}
