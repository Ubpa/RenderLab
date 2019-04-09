#ifndef _APP_RTGI_WITH_RRF_MODEL_H_
#define _APP_RTGI_WITH_RRF_MODEL_H_

#include <CppUtil/Basic/HeapObj.h>

#include <vector>
#include <string>

namespace App {
	class Layer;

	class Model : public CppUtil::Basic::HeapObj {
	public:
		explicit Model(const std::string & name = "",
			const int inputDim = -1, const int outputDim = -1)
			: name(name), inputDim(inputDim), outputDim(outputDim) { }

	public:
		static const CppUtil::Basic::Ptr<Model> New(const std::string & name = "",
			const int inputDim = -1, const int outputDim = -1) {
			return CppUtil::Basic::New<Model>(name, inputDim, outputDim);
		}

	protected:
		virtual ~Model() = default;

	public:
		const std::string GetFuncName() const { return name; }
		const std::vector<CppUtil::Basic::PtrC<Layer>> & GetLayers() const { return layers; }
		int GetInputDim() const { return inputDim; }
		int GetOutputDim() const { return outputDim; }
		int GetIDof(CppUtil::Basic::PtrC<Layer> layer) const;
		bool IsValid() const;

	public:
		bool AddLayer(CppUtil::Basic::PtrC<Layer> layer) const;
		const std::string GenFunc(bool genLayers = true) const;

	private:
		mutable std::vector<CppUtil::Basic::PtrC<Layer>> layers;

		int inputDim;
		int outputDim;

		std::string name;
	};
}

#endif//!_APP_RTGI_WITH_RRF_MODEL_H_
