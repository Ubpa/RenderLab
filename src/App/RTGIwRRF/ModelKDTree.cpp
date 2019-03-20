#include "ModelKDTree.h"

#include "Model.h"

#include <CppUtil/Basic/Error.h>

#include <sstream>

using namespace App;
using namespace CppUtil::Basic;
using namespace std;

const std::string ModelKDTree::GenFunc(bool genModels) const {
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
	GenFuncRecursion(genModels, sstreams, indent);
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

	if (IsLeaf()) {
		sstreams.kdTree << indent << GetData()->GetFuncName() << "(";
		for (int i = 0; i < inputDim; i++)
			sstreams.kdTree << "x" << i << ",";
		for (int i = 0; i < outputDim; i++)
			sstreams.kdTree << "h" << i << (i != outputDim - 1 ? "," : "");
		sstreams.kdTree << ");" << endl;
	}
	else {
		sstreams.kdTree << indent << "if ( x" << (3 + GetAxis()) << " < " << GetSpiltVal() << " ) {" << endl;
		if (!GetLeft()->GenFuncRecursion(genModels, sstreams, indent + "    "))
			return ERROR;
		sstreams.kdTree << indent << "}" << endl;
		sstreams.kdTree << indent << "else {" << endl;
		if (!GetRight()->GenFuncRecursion(genModels, sstreams, indent + "    "))
			return ERROR;
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
		|| (!IsLeaf() && (GetData() || GetAxis() < 0 || GetAxis() > 2 || !GetLeft() || !GetRight()))
		|| (IsLeaf() && !GetData())
		)
	{
		return ERROR;
	}
	
	return true;
}
