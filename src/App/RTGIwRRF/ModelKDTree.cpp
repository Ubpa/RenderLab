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

	rst << GenFuncRecursion();

	// func declaration
	rst << "void Model";
	rst << endl << "(" << endl;
	rst << indent << "// input" << endl;
	for (int i = 0; i < inputDim; i++)
		rst << indent << "in float x" << i << "," << endl;
	rst << indent << endl;
	rst << indent << "// output" << endl;
	for (int i = 0; i < outputDim; i++) {
		rst << indent << "out float h" << i;
		if (i == outputDim - 1)
			rst << endl << ")" << endl;
		else
			rst << "," << endl;
	}

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
	rst << indent << GetFuncName() << GenCallArgList("x", "h") << endl;

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
	rst << "void " << GetFuncName();
	rst << endl << "(" << endl;
	rst << indent << "// input" << endl;
	for (int i = 0; i < inputDim; i++)
		rst << indent << "in float x" << i << "," << endl;
	rst << indent << endl;
	rst << indent << "// output" << endl;
	for (int i = 0; i < outputDim; i++) {
		rst << indent << "out float h" << i;
		if (i == outputDim - 1)
			rst << endl << ")" << endl;
		else
			rst << "," << endl;
	}

	// func definition
	rst << "{" << endl;

	// kdTree
	rst << indent << "// KDTree" << endl;
	if (IsLeaf()) {
		rst << indent << GetData()->GetFuncName() << "(";
		for (int i = 0; i < inputDim; i++)
			rst << "x" << i << ",";
		for (int i = 0; i < outputDim; i++)
			rst << "h" << i << (i != outputDim - 1 ? "," : "");
		rst << ");" << endl;
	}
	else {
		const float axisExtent = GetSpiltAxisExtent();
		const float delta = axisExtent / 2 * interpolateRatio;
		const float lowBound = GetSpiltVal() - delta;
		const float highBound = GetSpiltVal() + delta;

		// left
		rst << indent << "if ( x" << GetAxis() << " < " << lowBound << " ) {" << endl;
		rst << indent << indent << GetLeft()->GetFuncName() << GenCallArgList("x", "h") << endl;
		rst << indent << "}" << endl;

		// interpolate
		const string leftPrefix = "left_h";
		const string rightPrefix = "right_h";
		rst << indent << "else if ( x" << GetAxis() << " < " << highBound << " ) {" << endl;
		// declare output
		for (int i = 0; i < outputDim; i++)
			rst << indent << indent << "float " << leftPrefix << i << ";" << endl;
		for (int i = 0; i < outputDim; i++)
			rst << indent << indent << "float " << rightPrefix << i << ";" << endl;
		rst << indent << indent << endl;
		// evaluate left and right
		rst << indent << indent << GetLeft()->GetFuncName() << GenCallArgList("x", leftPrefix) << endl;
		rst << indent << indent << GetRight()->GetFuncName() << GenCallArgList("x", rightPrefix) << endl;
		// smootherstep
		rst << indent << indent << endl;
		// ((6x-15)x+10)x^3
		rst << indent << indent << "float t = 0.5 + ( x" << GetAxis() << " - " << GetSpiltVal() << " ) / " << (axisExtent*interpolateRatio) << ";" << endl;
		rst << indent << indent << "t = ((6*t - 15)*t + 10) * t*t*t;" << endl;
		for (int i = 0; i < outputDim; i++) {
			rst << indent << indent << "h" << i <<
				" = ( 1 - t ) * " << leftPrefix << i << " + t * " << rightPrefix << i << ";" << endl;
		}
		rst << indent << "}" << endl;

		// right
		rst << indent << "else {" << endl;
		rst << indent << indent << GetRight()->GetFuncName() << GenCallArgList("x", "h") << endl;
		rst << indent << "}" << endl;
	}

	rst << endl;

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
