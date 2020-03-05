#ifndef _APP_RTGI_WITH_RRF_MODEL_H_
#define _APP_RTGI_WITH_RRF_MODEL_H_

#include <Basic/HeapObj.h>

#include <vector>
#include <string>

namespace App {
	class Layer;

	class Model : public HeapObj {
	public:
		explicit Model(const std::string & name = "",
			const int inputDim = -1, const int outputDim = -1)
			: name(name), inputDim(inputDim), outputDim(outputDim) { }

	public:
		static const Ptr<Model> New(const std::string & name = "",
			const int inputDim = -1, const int outputDim = -1) {
			return New<Model>(name, inputDim, outputDim);
		}

	protected:
		virtual ~Model() = default;

	public:
		const std::string GetFuncName() const { return name; }
		const std::vector<PtrC<Layer>> & GetLayers() const { return layers; }
		int GetInputDim() const { return inputDim; }
		int GetOutputDim() const { return outputDim; }
		int GetIDof(PtrC<Layer> layer) const;
		bool IsValid() const;

	public:
		bool AddLayer(PtrC<Layer> layer) const;
		const std::string GenFunc(bool genLayers = true) const;

	private:
		mutable std::vector<PtrC<Layer>> layers;

		int inputDim;
		int outputDim;

		std::string name;
	};
}

#endif//!_APP_RTGI_WITH_RRF_MODEL_H_
