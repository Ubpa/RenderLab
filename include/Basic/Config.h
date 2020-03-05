#pragma once

#include <Basic/File.h>
#include <Basic/LStorage.h>

namespace Ubpa {
	class Config {
	public:
		Config();
		Config(const std::string& fileName);
		bool Load(const std::string& fileName);
		const std::string* GetStrPtr(const std::string& id) const;
		const float* GetFloatPtr(const std::string& id) const;
		const int* GetIntPtr(const std::string& id) const;
		const unsigned int* GetUnsignedIntPtr(const std::string& id) const;
		bool GetV(const std::string& id, float& val, float defaultVal = 0) const;
		bool GetV(const std::string& id, int& val, int defaultVal = 0) const;
		bool GetV(const std::string& id, unsigned int& val, unsigned int defaultVal = 0) const;
		bool GetV(const std::string& id, std::string& val, const std::string& defaultVal = "") const;
		bool IsValid() const;
	private:
		bool DecodeLine(const std::string& data);
		//------------
		LStorage<std::string, std::string> strDirectory;
		LStorage<std::string, float> floatDirectory;
		LStorage<std::string, int> intDirectory;
		LStorage<std::string, unsigned int> unsignedIntDirectory;
		bool valid;
	};
}
