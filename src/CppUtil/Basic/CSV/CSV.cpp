#include <CppUtil/Basic/CSV.h>

using namespace CppUtil::Basic;
using namespace std;

template<typename ValT>
std::string CSV<ValT>::GetPlaceholder() {
	return "([%i]Unknow Val Type)";
}

template<>
std::string CSV<int>::GetPlaceholder() {
	return "%i";
}

template<>
std::string CSV<unsigned int>::GetPlaceholder() {
	return "%u";
}

template<>
std::string CSV<float>::GetPlaceholder() {
	return "%f";
}

template<>
std::string CSV<double>::GetPlaceholder() {
	return "%f";
}
