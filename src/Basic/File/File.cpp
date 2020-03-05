#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#endif // WIN32

#include <Basic/File.h>

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

using namespace std;

using namespace Ubpa;

File::File(const std::string & fileName, Mode mode) {
	this->fileName = fileName;
	this->mode = mode;
	pF = fopen(fileName.c_str(), ModeToStr(mode).c_str());
	if (pF == NULL)
		fprintf(stderr, "%s open error\n", fileName.c_str());
}

File::~File() {
	if (pF != NULL) {
		fclose(pF);
		pF = NULL;
	}
}

bool File::Check(Mode mode) const {
	if (mode == this->mode && IsValid())
		return true;
	else
		return false;
}

int File::Printf(const char * format, ...) {
	if (!Check(WRITE)) {
		fprintf(stderr, "%s can't not write\n", fileName.c_str());
		return -1;
	}
	va_list args;
	va_start(args, format);
	int rst = vfprintf(pF, format, args);
	va_end(args);
	return rst;
}

int File::Scanf(const char *  format, ...) const {
	if (!Check(READ)) {
		fprintf(stderr, "%s can't not read\n", fileName.c_str());
		return -1;
	}
	va_list args;
	va_start(args, format);
	int rst = vfscanf(pF, format, args);
	va_end(args);
	return rst;
}

string File::ReadLine() const {
	if (!Check(READ)) {
		fprintf(stderr, "%s can't not read\n", fileName.c_str());
		return "";
	}
	char buffer[1024] = "";
	fgets(buffer, 1024, pF);
	return buffer;
}

string File::ReadAll() const {
	if (!Check(READ)) {
		fprintf(stderr, "%s can't not read\n", fileName.c_str());
		return "";
	}
	string rst;
	while (!IsEnd())
		rst += ReadLine();
	return rst;
}

vector<string> File::ReadAllLines() const {
	vector<string> rst;

	if (!Check(READ)) {
		fprintf(stderr, "%s can't not read\n", fileName.c_str());
		return rst;
	}

	while (!IsEnd())
		rst.push_back(ReadLine());

	return rst;
}

bool File::IsEnd() const {
	return feof(pF);
}

bool File::IsValid() const {
	return pF != NULL;
}

void File::Close() {
	if (!IsValid())
		return;
	
	fclose(pF);
	pF = NULL;
}

string File::ModeToStr(Mode mode) {
	switch (mode)
	{
	case READ:
		return "r";
		break;
	case WRITE:
		return "w";
		break;
	default:
		return "";
		break;
	}
}

const string File::ReadAll(const std::string & fileName) {
	File file(fileName, Mode::READ);
	return file.ReadAll();
}

const std::vector<std::string> File::ReadAllLines(const std::string & fileName) {
	File file(fileName, Mode::READ);
	return file.ReadAllLines();
}

bool File::IsSame(const std::string & fileName0, const std::string & fileName1) {
	auto str0 = ReadAll(fileName0);
	auto str1 = ReadAll(fileName1);
	return str0 == str1;
}
