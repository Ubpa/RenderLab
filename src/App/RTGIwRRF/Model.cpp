#include "Model.h"

#include "Layer.h"

#include <Basic/Error.h>

#include <sstream>

using namespace App;

using namespace std;

int Model::GetIDof(PtrC<Layer> layer) const {
	static constexpr int ERROR = -1;

	if (layer == nullptr) {
		printf("ERROR: layer is nullptr\n");
		return ERROR;
	}

	if (layer->GetModel() != This()) {
		printf("ERROR: layer's model is not this model\n");
		return ERROR;
	}

	for (int i = 0; i < layers.size(); i++) {
		if (layers[i] == layer)
			return i;
	}

	return ERROR;
}

bool Model::AddLayer(PtrC<Layer> layer) const {
	static constexpr bool ERROR = false;

	if (layer == nullptr) {
		printf("ERROR: layer is nullptr.\n");
		return ERROR;
	}

	if (layer->GetModel() != nullptr && layer->GetModel() != This()) {
		printf("ERROR: layer's model is not this model.\n");
		return ERROR;
	}

	if (layers.size() == 0) {
		layers.push_back(layer);
		return true;
	}

	if (layer->GetInputDim() != layers.back()->GetOutputDim()) {
		printf("ERROR: layer's input dim [%d] is not %d\n", layer->GetInputDim(), layers.back()->GetOutputDim());
		return ERROR;
	}

	layers.push_back(layer);
	layer->SetModel(This<Model>());

	return true;
}

const string Model::GenFunc(bool genLayers) const {
	static auto const & ERROR = ErrorRetVal(&Model::GenFunc);

	if (!IsValid()) {
		printf("ERROR: model is not valid\n");
		return ERROR;
	}

	stringstream rst;

	if (genLayers) {
		for (auto layer : layers) {
			auto layerFunc = layer->GenFunc();
			if (IsError(layerFunc))
				return ERROR;

			rst << layerFunc << endl;
		}
	}

	const string indent = "    ";

	// func declaration
	rst << "void " << GetFuncName() << "(";
	for (int i = 0; i < inputDim; i++)
		rst << "float x" << i << ",";
	rst << endl << indent;
	for (int i = 0; i < outputDim; i++) {
		rst << "out float h_" << layers.size() << "_" << i;
		if (i == outputDim - 1)
			rst << ")" << endl;
		else
			rst << ",";
	}

	// func definition
	rst << "{" << endl;

	// layer
	for (int i = 0; i < layers.size(); i++) {
		auto const layer = layers[i];

		// declare output
		if (i < layers.size() - 1) {
			rst << indent << "float ";
			for (int j = 0; j < layer->GetOutputDim(); j++)
				rst << "h_" << (i + 1) << "_" << j << (j<layer->GetOutputDim()-1?",":";");
			rst << endl;
		}

		// call layer func
		rst << indent << layer->GetFuncName() << "(";

		// call layer func -- input
		if (i == 0) {
			for (int j = 0; j < layer->GetInputDim(); j++)
				rst << "x" << j << ",";
		}
		else {
			for (int j = 0; j < layer->GetInputDim(); j++)
				rst << "h_" << i << "_" << j << ",";
		}

		// call layer func -- output
		for (int j = 0; j < layer->GetOutputDim(); j++) {
			rst << "h_" << (i + 1) << "_" << j;
			if (j < layer->GetOutputDim() - 1)
				rst << ",";
		}

		rst << ");" << endl << endl;
	}

	rst << "}" << endl;

	return rst.str();
}

bool Model::IsValid() const {
	static constexpr bool ERROR = false;

	if (layers.size() == 0
		|| inputDim == -1
		|| outputDim <= 0
		|| layers.back()->GetOutputDim() != outputDim
		|| layers.front()->GetInputDim() != inputDim)
	{
		return ERROR;
	}

	for (int i = 1; i < layers.size(); i++) {
		if (layers[i]->GetInputDim() != layers[i - 1]->GetOutputDim())
			return ERROR;
	}

	return true;
}
