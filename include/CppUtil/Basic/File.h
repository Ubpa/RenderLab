#ifndef _BASIC_FILE_FILE_H_
#define _BASIC_FILE_FILE_H_

#include <string>
#include <vector>

namespace CppUtil {
	namespace Basic {
		class File {
		public:
			enum Mode {
				READ,
				WRITE
			};

			File(const std::string & fileName, Mode mode);

			~File();

			bool Check(Mode mode) const;

			int Printf(const char * format, ...);

			int Scanf(const char * format, ...) const;

			std::string ReadLine() const;

			std::string ReadAll() const;
			std::vector<std::string> ReadAllLines() const;

			bool IsEnd() const;

			bool IsValid() const;

			void Close();
		private:
			std::string ModeToStr(Mode mode);

			FILE * pF;
			Mode mode;
			std::string fileName;
		};
	};
}
#endif//! _BASIC_FILE_FILE_H_