#include <CppUtil/OpenGL/Shader.h>

#include <iostream>

using namespace std;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;

size_t Shader::curID = 0;

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
	size_t glvs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(glvs, 1, &vsCStr, NULL);
	glCompileShader(glvs);
	if (!CheckCompileErrors(glvs, "VERTEX")) {
		cout << vertexPath << " compiles error\n";
		valid = false;
		return;
	}

	// fragment Shader
	size_t glfs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(glfs, 1, &fsCStr, NULL);
	glCompileShader(glfs);
	if(!CheckCompileErrors(glfs, "FRAGMENT")) {
		cout << fragmentPath << " compiles error\n";
		valid = false;
		return;
	}

	// geometry Sahder
	size_t glgs = 0;
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
		if (!CheckCompileErrors(glgs, "GEOMETRY")) {
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
	if (!CheckCompileErrors(ID, "PROGRAM")) {
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

size_t Shader::GetID() const { return ID; }
bool Shader::IsValid() const { return valid; }

bool Shader::Use() const{
	if (valid && curID != ID) {
		glUseProgram(ID);
		curID = ID;
	}
	return valid;
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

void Shader::SetFloats(const string &name, size_t n, const float * values) const {
	Use();
	glUniform1fv(glGetUniformLocation(ID, name.c_str()), n, values);
}


void Shader::SetVec2f(const std::string &name, float value0, float value1) const {
	Use();
	glUniform2f(glGetUniformLocation(ID, name.c_str()), value0, value1);
}

void Shader::SetVec2f(const std::string &name, const glm::vec2 & v) const {
	SetVec2f(name, v.x, v.y);
}

void Shader::SetVec3f(const string &name, float value0, float value1, float value2) const {
	Use();
	glUniform3f(glGetUniformLocation(ID, name.c_str()), value0, value1, value2);
}

void Shader::SetVec3f(const string &name, const glm::vec3 & v) const {
	SetVec3f(name, v.x, v.y, v.z);
}

void Shader::SetVec3fs(const std::string &name, size_t n, const float * data) const {
	Use();
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 4, data);
}

void Shader::SetVec4f(const string &name, float value0, float value1, float value2, float value3) const{
	Use();
	glUniform4f(glGetUniformLocation(ID, name.c_str()), value0, value1, value2, value3);
}

void Shader::SetVec4f(const string &name, const glm::vec4 & v) const {
	SetVec4f(name, v.x, v.y, v.z, v.w);
}

void Shader::SetMat4f(const string &name, const float * matValue) const{
	Use();
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, matValue);
}

void Shader::SetMat4f(const string &name, const glm::mat4 mat4) const{
	SetMat4f(name, glm::value_ptr(mat4));
}

void Shader::UniformBlockBind(const string &name, size_t bindPoint) {
	glUniformBlockBinding(ID, glGetUniformBlockIndex(ID, name.c_str()), bindPoint);
}

int Shader::CheckCompileErrors(size_t shader, string type){
	int success;
	char infoLog[1024];
	if (type != "PROGRAM"){
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success){
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << endl;
		}
	}else{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << endl;
		}
	}
	return success;
}