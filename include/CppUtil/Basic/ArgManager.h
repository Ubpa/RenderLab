#ifndef _ARG_MANAGER_H_
#define _ARG_MANAGER_H_

#include <string>
#include <vector>
#include <map>

namespace CppUtil {
	namespace Basic {
		class ArgManager {
		public:
			ArgManager(int argc, char** argv);
			std::vector<std::string> GetAll(const std::string & key);
			std::string Get(const std::string & key, size_t idx, const std::string & defaultValue = "");
		private:
			std::map<std::string, std::vector<std::string>> pairs;
		};
	};
}
#endif//! _ARG_MANAGER_H_