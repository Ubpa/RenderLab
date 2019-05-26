#ifndef _APP_RTGI_WITH_RRF_UNIT_H_
#define _APP_RTGI_WITH_RRF_UNIT_H_

#include <CppUtil/Basic/HeapObj.h>

#include <3rdParty/enum.h>

#include <vector>
#include <string>

namespace App {
	class Layer;

	class Unit : public CppUtil::Basic::HeapObj {
	public:
		explicit Unit(CppUtil::Basic::PtrC<Layer> layer = nullptr,
			const std::vector<float> & weights = std::vector<float>())
			: layer(layer), weights(weights) { }

	public:
		static const CppUtil::Basic::Ptr<Unit> New(CppUtil::Basic::PtrC<Layer> layer = nullptr,
			const std::vector<float> & weights = std::vector<float>()) {
			return CppUtil::Basic::New<Unit>(layer, weights);
		}

	protected:
		virtual ~Unit() = default;

	public:
		int GetInputDim() const { return static_cast<int>(weights.size()) - 1; }
		const std::vector<float> & GetWeights() const { return weights; }
		const std::string GetFuncName() const;
		int GetID() const;
		const CppUtil::Basic::PtrC<Layer> GetLayer() const { return layer.lock(); }
		bool SetLayer(CppUtil::Basic::PtrC<Layer> layer) const;
		bool IsValid() const;

	public:
		const std::string GenComputeExpr() const;

	private:
		virtual void Init_AfterGenPtr() override;

	private:
		mutable CppUtil::Basic::WPtrC<Layer> layer;
		std::vector<float> weights;
	};
}

#endif//!_APP_RTGI_WITH_RRF_UNIT_H_
