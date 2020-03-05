#pragma once

#include <string>
#include <vector>

namespace Ubpa {
	class File {
	public:
		enum Mode {
			READ,
			WRITE
		};

		File(const std::string& fileName, Mode mode);

		~File();

		bool Check(Mode mode) const;

		int Printf(const char* format, ...);

		int Scanf(const char* format, ...) const;

		std::string ReadLine() const;

		std::string ReadAll() const;
		std::vector<std::string> ReadAllLines() const;

		bool IsEnd() const;

		bool IsValid() const;

		void Close();

	public: // static
		static const std::string ReadAll(const std::string& fileName);
		static const std::vector<std::string> ReadAllLines(const std::string& fileName);
		static bool IsSame(const std::string& fileName0, const std::string& fileName1);

	private:
		std::string ModeToStr(Mode mode);

		FILE* pF;
		Mode mode;
		std::string fileName;
	};
}
