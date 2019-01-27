#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <CppUtil/Basic/File.h>
#include <CppUtil/Basic/LStorage.h>

namespace CppUtil {
	namespace Basic {
		class Config {
		public:
			Config();
			Config(const std::string & fileName);
			bool Load(const std::string & fileName);
			const std::string * GetStrPtr(const std::string & id) const;
			const float * GetFloatPtr(const std::string & id) const;
			const int * GetIntPtr(const std::string & id) const;
			const unsigned int * GetUnsignedIntPtr(const std::string & id) const;
			bool GetVal(const std::string & id, float & val, float defaultVal = 0) const;
			bool GetVal(const std::string & id, int & val, int defaultVal = 0) const;
			bool GetVal(const std::string & id, unsigned int & val, unsigned int defaultVal = 0) const;
			bool GetVal(const std::string & id, std::string & val, const std::string & defaultVal = "") const;
			bool IsValid() const;
		private:
			bool Config::DecodeLine(const std::string & data);
			//------------
			LStorage<std::string> strDirectory;
			LStorage<float> floatDirectory;
			LStorage<int> intDirectory;
			LStorage<unsigned int> unsignedIntDirectory;
			bool valid;
		};
	}
}
#endif // !_CONFIG_H_
