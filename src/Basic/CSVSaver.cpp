#include <Basic/CSVSaver.h>

using namespace Ubpa;

using namespace std;

template<typename ValT>
string CSVSaver<ValT>::GetPlaceholder() {
	return "([%i]Unknow Val Type)";
}

template<>
string CSVSaver<int>::GetPlaceholder() {
	return "%i";
}

template<>
string CSVSaver<unsigned int>::GetPlaceholder() {
	return "%u";
}

template<>
string CSVSaver<float>::GetPlaceholder() {
	return "%f";
}

template<>
string CSVSaver<double>::GetPlaceholder() {
	return "%f";
}
