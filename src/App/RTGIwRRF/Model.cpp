#include "Model.h"

#include "Layer.h"

#include <CppUtil/Basic/Error.h>

#include <sstream>

using namespace App;
using namespace CppUtil::Basic;
using namespace std;

Model::Model(const std::string & name,
	const int inputDim, const int outputDim,
	const std::vector<float> & means,
	const std::vector<float> & stds)
	: name(name), inputDim(inputDim), outputDim(outputDim), means(means), stds(stds)
{
	int sumDim = inputDim + outputDim;
	if (sumDim != means.size() || sumDim != stds.size()) {
		this->means.clear();
		this->stds.clear();
		this->inputDim = -1;
		this->outputDim = -1;
		this->name = "";
		return;
	}
}

int Model::GetIDof(Layer::CPtr layer) const {
	static constexpr int ERROR = -1;

	if (layer == nullptr) {
		printf("ERROR: layer is nullptr\n");
		return ERROR;
	}

	if (layer->GetModel() != CThis()) {
		printf("ERROR: layer's model is not this model\n");
		return ERROR;
	}

	for (int i = 0; i < layers.size(); i++) {
		if (layers[i] == layer)
			return i;
	}

	return ERROR;
}

bool Model::AddLayer(Layer::CPtr layer) const {
	static constexpr bool ERROR = false;

	if (layer == nullptr) {
		printf("ERROR: layer is nullptr.\n");
		return ERROR;
	}

	if (layer->GetModel() != nullptr && layer->GetModel() != CThis()) {
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
	layer->SetModel(CThis());

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
	rst << "void " << GetFuncName();
	rst << endl << "(" << endl;
	for (int i = 0; i < inputDim; i++)
		rst << indent << "in float x" << i << "," << endl;
	rst << "    " << endl;
	for (int i = 0; i < outputDim; i++) {
		rst << indent << "out float h_" << layers.size() << "_" << i;
		if (i == outputDim - 1)
			rst << endl << ")" << endl;
		else
			rst << "," << endl;
	}

	// func definition
	rst << "{" << endl;

	// normalization
	rst << indent << "// normalization" << endl << endl;
	for (int i = 0; i < inputDim; i++) {
		rst << indent << "float h_0_" << i << " = ";
		rst << "(x" << i << " - (" << means[i] << ")) / (" << stds[i] << ");" << endl;
	}
	rst << endl;

	// layer
	for (int i = 0; i < layers.size(); i++) {
		auto const layer = layers[i];

		rst << indent << "// ------------------------ [ layer " << i << " ]" << endl << endl;

		// output
		if (i < layers.size() - 1) {
			for (int j = 0; j < layer->GetOutputDim(); j++)
				rst << indent << "float h_" << (i + 1) << "_" << j << ";" << endl;
			rst << indent << endl;
		}

		// call layer func
		rst << indent << layer->GetFuncName() << endl;
		rst << indent << "(" << endl;
		rst << indent << indent << "// input" << endl;
		for (int j = 0; j < layer->GetInputDim(); j++)
			rst << indent << indent << "h_" << i << "_" << j << "," << endl;
		rst << indent << indent << endl;
		rst << indent << indent << "// output" << endl;
		for (int j = 0; j < layer->GetOutputDim(); j++) {
			rst << indent << indent << "h_" << (i + 1) << "_" << j;
			if (j < layer->GetOutputDim() - 1)
				rst << "," << endl;
			else
				rst << endl;
		}
		rst << indent << ");" << endl << endl;
	}

	// de nomalization
	rst << indent << "// Denormalization" << endl << endl;
	for (int i = 0; i < outputDim; i++) {
		rst << indent << "h_" << layers.size() << "_" << i << " = ";
		rst << "h_" << layers.size() << "_" << i << " * (" << stds[inputDim + i] << ") + (" << means[inputDim + i] << ")";
		rst << ";" << endl;
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
