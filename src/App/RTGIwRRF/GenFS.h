#ifndef _APP_RTGI_W_RRF_GEN_FS_H_
#define _APP_RTGI_W_RRF_GEN_FS_H_

#include "Layer.h"

namespace App {
	class GenFS {
	public:
		static const std::string Call(const std::string & meanAndStdPath,
			const std::string & dense0Path, const Activation & activation0,
			const std::string & dense1Path, const Activation & activation1,
			const std::string & dense2Path, const Activation & activation2);
	private:
		typedef std::vector<std::vector<float>> Matf;

		static const std::vector<std::vector<float>> LoadCSV(const std::string & path);
		static const Matf LoadMatrix(const std::string & path);
		static const Layer::CPtr LoadLayer(const std::string & path, const Connection & connection, const Activation & activation);

	private:
		GenFS() = default;
	};
}

#endif//!_APP_RTGI_W_RRF_GEN_FS_H_
