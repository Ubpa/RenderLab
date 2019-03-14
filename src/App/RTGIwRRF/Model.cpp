#include "Model.h"

#include "Layer.h"

#include <CppUtil/Basic/Error.h>

#include <sstream>

using namespace App;
using namespace CppUtil::Basic;
using namespace std;

int Model::GetIDof(CppUtil::Basic::CPtr<Layer> layer) const {
	static constexpr int ERROR = -1;

	if (layer == nullptr)
		return -1;

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

	if (layer->GetInputDim() != GetOutputDim()) {
		printf("ERROR: layer's input dim [%d] is not %d\n", layer->GetInputDim(), GetOutputDim());
		return ERROR;
	}

	layers.push_back(layer);
	layer->SetModel(CThis());

	return true;
}

int Model::GetInputDim() const {
	return layers.size() > 0 ? layers[0]->GetInputDim() : -1;
}

int Model::GetOutputDim() const {
	return layers.size() > 0 ? layers.back()->GetOutputDim() : -1;
}


const string Model::GenFunc(bool genLayers) const {
	static auto const & ERROR = ErrorRetVal(&Model::GenFunc);

	stringstream modelFunc;

	if (genLayers) {
		for (auto layer : layers) {
			auto layerFunc = layer->GenFunc();
			if (IsError(layerFunc))
				return ERROR;

			modelFunc << layerFunc << endl;
		}
	}

	return modelFunc.str();
}
