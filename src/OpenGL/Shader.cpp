#include <OpenGL/Shader.h>

#include <Basic/File.h>

#include <Basic/StrAPI.h>

// Choose OpenGL API
#if USE_QT_OPENGL_API
#include <Qt/RawAPI_Define.h>
#else
#include <glad/glad.h>
#endif

#include <iostream>

using namespace Ubpa;
using namespace std;

const Shader Shader::InValid = Shader();

Shader::Shader() : valid(false){ }

Shader::Shader(const string & vertexPath, const string & fragmentPath, const string & geometryPath) {
	valid = true;
	File vsF(vertexPath, File::Mode::READ);
	File fsF(fragmentPath, File::Mode::READ);
	bool hasGS = geometryPath.size() > 0;

	string vsStr = Process(vertexPath);
	string fsStr = Process(fragmentPath);
	if (vsStr.size() == 0 || fsStr.size() == 0) {
		cout << vertexPath << " or " << fragmentPath << " read failed.\n";
		valid = false;
		return;
	}
	const char * vsCStr = vsStr.c_str();
	const char * fsCStr = fsStr.c_str();

	// vertex shader
	unsigned glvs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(glvs, 1, &vsCStr, NULL);
	glCompileShader(glvs);
	if (!CheckCompileErrors(glvs, CompileType::VERTEX)) {
		cout << vertexPath << " compiles error\n";
		valid = false;
		return;
	}

	// fragment Shader
	unsigned glfs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(glfs, 1, &fsCStr, NULL);
	glCompileShader(glfs);
	if(!CheckCompileErrors(glfs, CompileType::FRAGMENT)) {
		cout << fragmentPath << " compiles error\n";
		valid = false;
		return;
	}

	// geometry Sahder
	unsigned glgs = 0;
	if (hasGS) {
		string gsStr = Process(geometryPath);
		if (gsStr.size() == 0) {
			cout << geometryPath << " read failed.\n";
			valid = false;
			return;
		}

		const char * gsCStr = gsStr.c_str();
		glgs = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(glgs, 1, &gsCStr, NULL);
		glCompileShader(glgs);
		if (!CheckCompileErrors(glgs, CompileType::GEOMETRY)) {
			cout << geometryPath << " compiles error\n";
			valid = false;
			return;
		}
	}

	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, glvs);
	glAttachShader(ID, glfs);
	if(hasGS)
		glAttachShader(ID, glgs);
	glLinkProgram(ID);
	if (!CheckCompileErrors(ID, CompileType::PROGRAM)) {
		if(hasGS)
			cout << vertexPath << ", " << fragmentPath << " and " << geometryPath << " link failed.\n";
		else
			cout << vertexPath << " and " << fragmentPath << " link failed.\n";
		valid = false;
		return;
	}
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(glvs);
	glDeleteShader(glfs);
	glDeleteShader(glgs);
}

unsigned Shader::GetID() const { return ID; }
bool Shader::IsValid() const { return valid; }

bool Shader::Use() const{
	if (!valid) {
		printf("ERROR::Shader::Use:\n"
			"\t""use an invalid shader\n");
		return false;
	}
	
	glUseProgram(ID);
	return true;
}

void Shader::SetBool(const string &name, bool value) const{
	Use();
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::SetInt(const string &name, int value) const{
	Use();
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetFloat(const string &name, float value) const{
	Use();
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetFloats(const string &name, unsigned n, const float * values) const {
	Use();
	glUniform1fv(glGetUniformLocation(ID, name.c_str()), n, values);
}

void Shader::SetVecf2(const std::string &name, float value0, float value1) const {
	Use();
	glUniform2f(glGetUniformLocation(ID, name.c_str()), value0, value1);
}

void Shader::SetVecf3(const string &name, float value0, float value1, float value2) const {
	Use();
	glUniform3f(glGetUniformLocation(ID, name.c_str()), value0, value1, value2);
}

void Shader::SetVecf3s(const std::string &name, unsigned n, const float * data) const {
	Use();
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 4, data);
}

void Shader::SetVec4f(const string &name, float value0, float value1, float value2, float value3) const{
	Use();
	glUniform4f(glGetUniformLocation(ID, name.c_str()), value0, value1, value2, value3);
}

void Shader::SetMatf4(const string &name, const float * matValue) const{
	Use();
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, matValue);
}

void Shader::UniformBlockBind(const string &name, unsigned bindPoint) const {
	glUniformBlockBinding(ID, glGetUniformBlockIndex(ID, name.c_str()), bindPoint);
}

int Shader::CheckCompileErrors(unsigned shader, CompileType type){
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	char infoLog[1024];

	string shaderName;
	switch (type)
	{
	case Ubpa::Shader::CompileType::VERTEX:
		shaderName = "VERTEX";
		break;
	case Ubpa::Shader::CompileType::GEOMETRY:
		shaderName = "GEOMETRY";
		break;
	case Ubpa::Shader::CompileType::FRAGMENT:
		shaderName = "FRAGMENT";
		break;
	case Ubpa::Shader::CompileType::PROGRAM:
		shaderName = "PROGRAM";
		break;
	default:
		break;
	}

	switch (type)
	{
	case CompileType::VERTEX:
	case CompileType::GEOMETRY:
	case CompileType::FRAGMENT: {
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << shaderName << endl
				<< infoLog
				<< "\n -- --------------------------------------------------- -- " << endl;
		}
		break;
	}
	case CompileType::PROGRAM: {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << shaderName << endl
				<< infoLog
				<< "\n -- --------------------------------------------------- -- " << endl;
		}
		break;
	}
	default: {
		printf("ERROR::Shader::CheckCompileErrors:\n"
			"\t""not support CompileType\n");
		break;
	}
	}

	return success;
}

const string Shader::Process(const string & path) {
	File shaderFile(path, File::Mode::READ);
	vector<string> contents = shaderFile.ReadAllLines();

	string root = StrAPI::DelTailAfter(path, '/') + "/";
	string name = StrAPI::TailAfter(path, '/');

	string rst;

	for (const auto & line : contents) {
		if (StrAPI::IsBeginWith(line, "#include")) {
			string incPath = StrAPI::Between(line, '"', '"');
			string inc = Process(root + incPath);
			if (inc.empty()) {
				printf("[WARNING] Shader::Process:\n"
					"\t""#include is empty or fail\n"
					"\t""code: %s"
					"\t""path: %s\n", line.c_str(), incPath.c_str());
				continue;
			}

			rst += inc;
		}else
			rst += line;
	}

	File debugShaderFile(root + "_DEBUG_" + name, File::Mode::WRITE);
	debugShaderFile.Printf("%s", rst.c_str());
	debugShaderFile.Close();

	return rst;
}
