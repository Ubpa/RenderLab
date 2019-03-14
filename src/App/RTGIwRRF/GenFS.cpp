#include "GenFS.h"

#include "Model.h"
#include "Layer.h"
#include "Unit.h"

#include <CppUtil/Basic/Math.h>
#include <CppUtil/Basic/Error.h>

#include <3rdParty/csv/CSV.hpp>

#include <sstream>

using namespace App;
using namespace CppUtil::Basic;
using namespace jay::util;
using namespace std;

const string GenFS::Call(const string & meanAndStdPath,
	const string & dense0Path, const Activation & activation0,
	const string & dense1Path, const Activation & activation1,
	const string & dense2Path, const Activation & activation2)
{
	static auto const & ERROR = ErrorRetVal(Call);

	constexpr int inputDim = 17;
	constexpr int outputDim = 3;

	auto meanAndStd = LoadMatrix(meanAndStdPath);
	if (IsError(meanAndStd))
		return ERROR;

	if (meanAndStd.size() != 2 || meanAndStd[0].size() != inputDim + outputDim) {
		printf("ERROR: row col error [meanAndStdPath]\n");
		return ERROR;
	}

	auto model = ToPtr(new Model("Model"));

	auto dense0 = LoadLayer(dense0Path, Connection::Dense, activation0);
	if (IsError(dense0))
		return ERROR;
	dense0->SetModel(model);

	auto dense1 = LoadLayer(dense1Path, Connection::Dense, activation1);
	if (IsError(dense1))
		return ERROR;
	dense1->SetModel(model);

	auto dense2 = LoadLayer(dense2Path, Connection::Dense, activation2);
	if (IsError(dense2))
		return ERROR;
	dense2->SetModel(model);

	stringstream shader;
	
	shader << model->GenFunc() << endl;

	return shader.str();
}

const vector<vector<float>> GenFS::LoadCSV(const string & path) {
	static auto const & ERROR = ErrorRetVal(LoadCSV);
	
	CSVread reader(path);
	if (reader.error) {
		printf("ERROR: %s\n", reader.error_msg.c_str());
		return ERROR;
	}

	vector<vector<float>> rst;

	while (reader.ReadRecord()) {
		rst.push_back(vector<float>());
		for (auto const & valStr : reader.fields) {
			float val = Math::CastTo<float>(valStr);
			rst.back().push_back(val);
		}
	}

	return rst;
}

const GenFS::Matf GenFS::LoadMatrix(const string & path) {
	static auto const & ERROR = ErrorRetVal(LoadMatrix);

	auto rst = LoadCSV(path);
	if (IsError(rst))
		return ERROR;

	for (auto const & row : rst) {
		if (row.size() != rst.front().size()) {
			printf("ERROR: not matrix [%s]\n", path.c_str());
			return ERROR;
		}
	}

	return rst;
}

const Layer::CPtr GenFS::LoadLayer(const string & path, const Connection & connection, const Activation & activation) {
	static auto const & ERROR = ErrorRetVal(LoadLayer);

	auto rst = LoadMatrix(path);
	if (IsError(rst))
		return ERROR;

	if (rst.size() == 0)
		return ERROR;

	size_t w = rst[0].size();
	size_t h = rst.size();

	// transpose
	vector<vector<float>> rstT(w);
	for (int row = 0; row < w; row++) {
		rstT[row] = vector<float>(h);
		for (int col = 0; col < h; col++)
			rstT[row][col] = rst[col][row];
	}

	auto layer = ToPtr(new Layer(nullptr, static_cast<int>(h) - 1, connection, activation));

	for (auto const & weights : rstT)
		auto unit = ToPtr(new Unit(layer, weights));

	return layer;
}
