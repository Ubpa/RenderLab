#ifndef _CPPUTIL_BASIC_CSV_CSVSaver_H_
#define _CPPUTIL_BASIC_CSV_CSVSaver_H_

#include <CppUtil/Basic/File.h>

#include <vector>
#include <string>

namespace CppUtil {
	namespace Basic {
		template<typename ValT>
		class CSVSaver {
		public:
			CSVSaver(const std::vector<std::string> & keys)
				: keys(keys) { }
			
			size_t GetKeyLength() const {
				return keys.size();
			}

			bool AddLine(const std::vector<ValT> & lineValues);
			
			bool Save(const std::string & path) const;

		public:
			static std::string GetPlaceholder();

		private:
			std::vector<std::string> keys;
			std::vector<std::vector<ValT>> values;
		};

		template <typename ValT>
		bool CSVSaver<ValT>::AddLine(const std::vector<ValT> & lineValues) {
			if (lineValues.size() != keys.size())
				return false;

			values.push_back(lineValues);
			return true;
		}

		template <typename ValT>
		bool CSVSaver<ValT>::Save(const std::string & path) const {
			File file(path, File::WRITE);
			if (!file.IsValid())
				return false;

			for (int i = 0; i < keys.size() - 1; i++)
				file.Printf("%s,", keys[i].c_str());

			file.Printf("%s\n", keys.back().c_str());
			
			std::string format = GetPlaceholder();
			for (auto & lineVals : values) {
				for(int i=0;i<lineVals.size()-1;i++)
					file.Printf((format+",").c_str(), lineVals[i]);

				file.Printf((format+"\n").c_str(), lineVals.back());
			}

			return true;
		}

	}
}

#endif//!_CPPUTIL_BASIC_CSV_CSVSaver_H_
