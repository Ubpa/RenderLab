#include <CppUtil/Basic/File.h>

#include <cstdlib>
#include <cstdarg>

using namespace std;
using namespace CppUtil::Basic;


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
