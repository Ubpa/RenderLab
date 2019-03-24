#ifndef _APP_RTGI_WITH_RRF_MODEL_KDTREE_H_
#define _APP_RTGI_WITH_RRF_MODEL_KDTREE_H_

#include <CppUtil/Basic/KDTree.h>

#include <vector>
#include <string>
#include <sstream>

namespace App {
	class Model;

	class ModelKDTree : public CppUtil::Basic::KDTree<ModelKDTree, CppUtil::Basic::Ptr<Model>> {
		HEAP_OBJ_SETUP(ModelKDTree)
	public:
		typedef CppUtil::Basic::Ptr<Model> ModelPtr;
		typedef CppUtil::Basic::KDTree<ModelKDTree, ModelPtr> BaseT;

	public:
		explicit ModelKDTree (
			const int ID = -1,
			ModelKDTree::Ptr parent = nullptr,
			const int inputDim = -1,
			const int outputDim = -1,
			const int axis = -1,
			const float spiltVal = 0,
			ModelPtr modelPtr = nullptr
		) :
			BaseT(parent, axis, spiltVal, modelPtr),
			ID(ID),
			inputDim(inputDim),
			outputDim(outputDim) { }

	public:
		int GetInputDim() const { return inputDim; }
		int GetOutputDim() const { return outputDim; }
		bool IsValid() const;
		float GetSpiltAxisExtent() const;
		const std::string GetFuncName() const;

	public:
		const std::string GenFunc(
			const std::vector<float> & minVal,
			const std::vector<float> & extent) const;

	private:
		const std::string GenFuncRecursion() const;

		// (x0, x1, ..., xn, h0, h1, ..., hm);
		const std::string GenCallArgList(const std::string & xName, const std::string & hName) const;

	private:
		int inputDim;
		int outputDim;
		int ID;

		static const float interpolateRatio;
	};
}

#endif//!_APP_RTGI_WITH_RRF_MODEL_KDTREE_H_
