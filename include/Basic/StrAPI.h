#pragma once

#include <string>
#include <vector>

namespace Ubpa {
	namespace StrAPI {
		const std::string Head(const std::string& str, int n);
		const std::string Tail(const std::string& str, int n);
		// exclusive
		const std::string TailAfter(const std::string& str, char c);

		bool IsBeginWith(const std::string& str, const std::string& suffix);
		bool IsEndWith(const std::string& str, const std::string& postfix);

		const std::vector<std::string> Spilt(const std::string& str, const std::string& separator);
		inline const std::vector<std::string> Spilt(const std::string& str, char separator) { return Spilt(str, std::string(1, separator)); }

		// rst = str[0] + separator + str[1] + separator + ... + str[strs.size()-1]
		const std::string Join(const std::vector<std::string>& strs, const std::string& separator = "");

		const std::string Replace(const std::string& str, const std::string& orig, const std::string& target);

		// inclusive
		const std::string DelTailAfter(const std::string& str, char c);

		const std::string Between(const std::string& str, char left, char right);

		const std::string LowerCase(const std::string& str);
		const std::string UpperCase(const std::string& str);
	}
}
