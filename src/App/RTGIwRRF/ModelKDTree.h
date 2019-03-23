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
			ModelKDTree::Ptr parent = nullptr,
			const int inputDim = -1,
			const int outputDim = -1,
			int axis = -1,
			float spiltVal = 0,
			ModelPtr modelPtr = nullptr
		) :
			BaseT(parent, axis, spiltVal, modelPtr),
			inputDim(inputDim),
			outputDim(outputDim) { }

	public:
		int GetInputDim() const { return inputDim; }
		int GetOutputDim() const { return outputDim; }
		bool IsValid() const;

	public:
		const std::string GenFunc(
			const std::vector<float> & minVal,
			const std::vector<float> & extent,
			bool genModels = true) const;

	private:
		struct SStreams {
			std::stringstream models;
			std::stringstream kdTree;
		};
		bool GenFuncRecursion(bool genModels, SStreams & sstreams, const std::string & indent) const;

	private:
		int inputDim;
		int outputDim;
	};
}

#endif//!_APP_RTGI_WITH_RRF_MODEL_KDTREE_H_
