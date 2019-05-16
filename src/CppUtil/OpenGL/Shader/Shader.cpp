#include <CppUtil/OpenGL/Shader.h>

#include <CppUtil/Basic/File.h>

// Choose OpenGL API
#if USE_QT_OPENGL_API
#include <CppUtil/Qt/RawAPI_Define.h>
#else
#include <glad/glad.h>
#endif

#include <iostream>

using namespace std;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;

Shader::Shader() : valid(false){ }

Shader::Shader(const string & vertexPath, const string & fragmentPath, const string & geometryPath) {
	valid = true;
	File vsF(vertexPath, File::Mode::READ);
	File fsF(fragmentPath, File::Mode::READ);
	bool hasGS = geometryPath.size() > 0;

	string vsStr = vsF.ReadAll();
	string fsStr = fsF.ReadAll();
	if (vsStr.size() == 0 || fsStr.size() == 0) {
		cout << vertexPath << " or " << fragmentPath << " read failed.\n";
		valid = false;
		return;
	}
	const char * vsCStr = vsStr.c_str();
	const char * fsCStr = fsStr.c_str();

	// vertex shader
	uint glvs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(glvs, 1, &vsCStr, NULL);
	glCompileShader(glvs);
	if (!CheckCompileErrors(glvs, CompileType::VERTEX)) {
		cout << vertexPath << " compiles error\n";
		valid = false;
		return;
	}

	// fragment Shader
	uint glfs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(glfs, 1, &fsCStr, NULL);
	glCompileShader(glfs);
	if(!CheckCompileErrors(glfs, CompileType::FRAGMENT)) {
		cout << fragmentPath << " compiles error\n";
		valid = false;
		return;
	}

	// geometry Sahder
	uint glgs = 0;
	if (hasGS) {
		File gsF(geometryPath, File::Mode::READ);
		string gsStr = gsF.ReadAll();
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

uint Shader::GetID() const { return ID; }
bool Shader::IsValid() const { return valid; }

bool Shader::Use() const{
	if (!valid) {
		printf("ERROR::Shader::Use:\n"
			"\t""use a invalid shader\n");
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

void Shader::SetFloats(const string &name, uint n, const float * values) const {
	Use();
	glUniform1fv(glGetUniformLocation(ID, name.c_str()), n, values);
}


void Shader::SetVec2f(const std::string &name, float value0, float value1) const {
	Use();
	glUniform2f(glGetUniformLocation(ID, name.c_str()), value0, value1);
}

void Shader::SetVec3f(const string &name, float value0, float value1, float value2) const {
	Use();
	glUniform3f(glGetUniformLocation(ID, name.c_str()), value0, value1, value2);
}

void Shader::SetVec3fs(const std::string &name, uint n, const float * data) const {
	Use();
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 4, data);
}

void Shader::SetVec4f(const string &name, float value0, float value1, float value2, float value3) const{
	Use();
	glUniform4f(glGetUniformLocation(ID, name.c_str()), value0, value1, value2, value3);
}

void Shader::SetMat4f(const string &name, const float * matValue) const{
	Use();
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, matValue);
}

void Shader::UniformBlockBind(const string &name, uint bindPoint) const {
	glUniformBlockBinding(ID, glGetUniformBlockIndex(ID, name.c_str()), bindPoint);
}

int Shader::CheckCompileErrors(uint shader, CompileType type){
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	char infoLog[1024];

	switch (type)
	{
	case CompileType::VERTEX:
	case CompileType::GEOMETRY:
	case CompileType::FRAGMENT: {
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type._to_string() << endl
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
			cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << endl
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
