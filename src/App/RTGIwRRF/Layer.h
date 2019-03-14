#ifndef _APP_RTGI_WITH_RRF_LAYER_H_
#define _APP_RTGI_WITH_RRF_LAYER_H_

#include <CppUtil/Basic/HeapObj.h>

#include <3rdParty/enum.h>

#include <vector>
#include <string>

namespace App {
	class Model;
	class Unit;

	BETTER_ENUM(Connection, int,
		Dense)

	BETTER_ENUM(Activation, int,
		Identity,
		ReLU)

	class Layer : public CppUtil::Basic::HeapObj {
		HEAP_OBJ_SETUP(Layer)
	public:
		explicit Layer(CppUtil::Basic::CPtr<Model> model = nullptr,
			int inputDim = -1,
			const Connection & connection = Connection::Dense,
			const Activation & activation = Activation::Identity)
			: model(model), inputDim(inputDim), connection(connection), activation(activation) { }

	public:
		const std::vector<CppUtil::Basic::CPtr<Unit>> & GetUnits() const { return units; }
		const Activation & GetActivation() const { return activation; }
		const Connection & GetConnection() const { return connection; }
		int GetInputDim() const { return inputDim; }
		int GetOutputDim() const { return static_cast<int>(units.size()); }
		const CppUtil::Basic::CPtr<Model> GetModel() const { return model.lock(); }
		bool SetModel(CppUtil::Basic::CPtr<Model> model) const;

		int GetIDof(CppUtil::Basic::CPtr<Unit> unit) const;
		int GetID() const;
		const std::string GetFuncName() const;

	public:
		bool AddUnit(CppUtil::Basic::CPtr<Unit> unit) const;
		const std::string GenFunc(bool genUnits = true) const;

	public:
		void InitAfterGenSharePtr();

	private:
		mutable CppUtil::Basic::WCPtr<Model> model;
		mutable std::vector<CppUtil::Basic::CPtr<Unit>> units;

		const int inputDim;
		const Connection connection;
		const Activation activation;
	};
}

#endif//!_APP_RTGI_WITH_RRF_LAYER_H_
