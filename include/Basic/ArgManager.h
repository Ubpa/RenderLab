#pragma once

#include <string>
#include <vector>
#include <map>

namespace Ubpa {
	class ArgManager {
	public:
		ArgManager(int argc, char** argv);
		std::vector<std::string> GetAll(const std::string& key);
		std::string Get(const std::string& key, size_t idx, const std::string& defaultValue = "");
	private:
		std::map<std::string, std::vector<std::string>> pairs;
	};
}
