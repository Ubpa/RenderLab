#include "ModelKDTree.h"

#include "Model.h"

#include <CppUtil/Basic/Error.h>

#include <sstream>

using namespace App;
using namespace CppUtil::Basic;
using namespace std;

const std::string ModelKDTree::GenFunc(
	const vector<float> & minVal,
	const vector<float> & extent,
	bool genModels) const
{
	static auto const & ERROR = ErrorRetVal(&ModelKDTree::GenFunc);

	if (!IsValid()) {
		printf("ERROR: Model KDTree is not valid\n");
		return ERROR;
	}

	const string indent = "    ";

	SStreams sstreams;

	// func declaration
	sstreams.kdTree << "void ModelKDTree";
	sstreams.kdTree << endl << "(" << endl;
	sstreams.kdTree << indent << "// input" << endl;
	for (int i = 0; i < inputDim; i++)
		sstreams.kdTree << indent << "in float x" << i << "," << endl;
	sstreams.kdTree << indent << endl;
	sstreams.kdTree << indent << "// output" << endl;
	for (int i = 0; i < outputDim; i++) {
		sstreams.kdTree << indent << "out float h" << i;
		if (i == outputDim - 1)
			sstreams.kdTree << endl << ")" << endl;
		else
			sstreams.kdTree << "," << endl;
	}

	// func definition
	sstreams.kdTree << "{" << endl;

	// map to unit hyperbox
	sstreams.kdTree << indent << "// map to unit hyperbox" << endl;
	for (int i = 0; i < inputDim; i++) {
		sstreams.kdTree << indent << "x" << i << " = ";
		sstreams.kdTree << "(x" << i << " - (" << minVal[i] << ")) / (" << extent[i] << ");" << endl;
	}
	sstreams.kdTree << endl;

	// kdTree
	sstreams.kdTree << indent << "// KDTree" << endl;
	GenFuncRecursion(genModels, sstreams, indent);
	sstreams.kdTree << endl;

	// map back
	sstreams.kdTree << indent << "// map back" << endl;
	for (int i = 0; i < outputDim; i++) {
		sstreams.kdTree << indent << "h" << i << " = ";
		sstreams.kdTree << "h" << i << " * (" << extent[i] << ") + (" << minVal[i] << ")";
		sstreams.kdTree << ";" << endl;
	}

	sstreams.kdTree << "}" << endl;

	return sstreams.models.str() + "\n" + sstreams.kdTree.str();
}

bool ModelKDTree::GenFuncRecursion(bool genModels, ModelKDTree::SStreams & sstreams, const string & indent) const {
	static constexpr bool ERROR = false;

	if (genModels && GetData() != nullptr) {
		auto model = GetData()->GenFunc();
		if (IsError(model))
			return ERROR;

		sstreams.models << model;
	}

	stringstream callFunc;

	if (!HasTwoChild()) {
		callFunc << indent << "    " << GetData()->GetFuncName() << "(";
		for (int i = 0; i < inputDim; i++)
			callFunc << "x" << i << ",";
		for (int i = 0; i < outputDim; i++)
			callFunc << "h" << i << (i != outputDim - 1 ? "," : "");
		callFunc << ");" << endl;
	}

	if (IsLeaf()) {
		sstreams.kdTree << callFunc.str();
	}
	else {
		sstreams.kdTree << indent << "if ( x" << GetAxis() << " < " << GetSpiltVal() << " ) {" << endl;
		if (GetLeft()) {
			auto rst = static_cast<bool>(GetLeft()->GenFuncRecursion(genModels, sstreams, indent + "    "));
			if (rst == false)
				return ERROR;
		}
		else
			sstreams.kdTree << callFunc.str();

		sstreams.kdTree << indent << "}" << endl;
		sstreams.kdTree << indent << "else {" << endl;

		if (GetRight()) {
			auto rst = static_cast<bool>(GetRight()->GenFuncRecursion(genModels, sstreams, indent + "    "));
			if (rst == false)
				return ERROR;
		}else
			sstreams.kdTree << callFunc.str();

		sstreams.kdTree << indent << "}" << endl;
	}

	return true;
}

bool ModelKDTree::IsValid() const {
	static constexpr bool ERROR = false;

	if ((GetLeft() != nullptr && !GetLeft()->IsValid())
		|| (GetRight() != nullptr && !GetRight()->IsValid())
		|| inputDim < 0
		|| outputDim < 0
		|| (HasSingleChild() && !GetData())
		|| (HasTwoChild() && (GetData() || GetAxis() < 0 || GetAxis() >= inputDim || GetSpiltVal() < 0 || GetSpiltVal() > 1))
		|| (IsLeaf() && !GetData())
		)
	{
		return ERROR;
	}

	return true;
}
