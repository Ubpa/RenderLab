#ifndef _APP_RTGI_W_RRF_GEN_FS_H_
#define _APP_RTGI_W_RRF_GEN_FS_H_

#include "Layer.h"

#include <3rdParty/rapidjson/document.h>

#include <3rdParty/enum.h>

#include <vector>

namespace App {
	class Layer;
	class Model;
	class ModelKDTree;

	BETTER_ENUM(KEY, int,
		nodeID,
		spiltAxis,
		spiltVal,
		left,
		right
	)

	class GenFS {
	public:
		static const std::string Call(
			const int ID,
			const std::string dir,
			const std::string templateFSPath,
			const std::vector<Connection> & connections,
			const std::vector<Activation> & activations);
	private:
		typedef std::vector<std::vector<float>> Matf;

		static const std::vector<std::vector<float>> LoadCSV(const std::string & path);

		static const Matf LoadMatrix(const std::string & path);

		static const CppUtil::Basic::Ptr<Layer> LoadLayer(const std::string & path, const Connection & connection, const Activation & activation);

		static const CppUtil::Basic::Ptr<Model> LoadModel(
			const int ID,
			const int secID,
			const std::string & dir,
			const std::vector<Connection> & connections,
			const std::vector<Activation> & activations);

		static const CppUtil::Basic::Ptr<ModelKDTree> LoadModelKDTree(
			const int id,
			const std::string & dir,
			const std::vector<Connection> & connections,
			const std::vector<Activation> & activations);

		static const CppUtil::Basic::Ptr<ModelKDTree> LoadModelKDTreeFromJson(
			const int id,
			const std::string & dir,
			rapidjson::Value::MemberIterator begin,
			rapidjson::Value::MemberIterator end,
			const std::vector<Connection> & connections,
			const std::vector<Activation> & activations);

	private:
		GenFS() = default;
	};
}

#endif//!_APP_RTGI_W_RRF_GEN_FS_H_
