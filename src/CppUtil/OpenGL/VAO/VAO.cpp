#include <CppUtil/OpenGL/VAO.h>

#include<CppUtil/OpenGL/Shader.h>

#include <glad/glad.h>

using namespace CppUtil::OpenGL;
using namespace std;

VAO::VAO(float const * data, size_t dataSize, const std::vector<size_t> & attrLen){
	if (data == NULL || dataSize == 0 || attrLen.size() == 0) {
		isValid = false;
		ID = 0;
		return;
	}

	glGenVertexArrays(1, &ID);
	glBindVertexArray(ID);

	size_t VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
	size_t patchLen = 0;
	for (auto & len : attrLen)
		patchLen += len;
	for (size_t i = 0, accu = 0; i < attrLen.size(); i++) {
		glVertexAttribPointer(i, attrLen[i], GL_FLOAT, GL_FALSE, patchLen * sizeof(float), (void*)(accu * sizeof(float)));
		glEnableVertexAttribArray(i);
		accu += attrLen[i];
	}
	attrNum = attrLen.size();
	//按照没有索引的情况设置 pointNum
	this->pointNum = dataSize / (sizeof(float) * patchLen);
	isValid = true;
	hasIndex = false;
}

VAO::VAO(float const * data, size_t dataSize, const std::vector<size_t> & attrLen, size_t const * index, size_t indexSize)
	: VAO(data, dataSize, attrLen){
	if (IsValid()) {
		pointNum = indexSize / sizeof(size_t);
		hasIndex = true;
		isValid = GenBindEBO(index, indexSize);
	}
}

VAO::VAO(const std::vector<VBO_DataPatch> & vec_VBO_DataPatch, const std::vector<size_t> & divisors) {
	if (vec_VBO_DataPatch.size() == 0
		|| (divisors.size() > 0 && vec_VBO_DataPatch.size() != divisors.size())) {
		isValid = false;
		return;
	}

	glGenVertexArrays(1, &ID);
	glBindVertexArray(ID);

	for (size_t i = 0; i < vec_VBO_DataPatch.size(); i++) {
		auto & dataPatch = vec_VBO_DataPatch[i];
		size_t VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, dataPatch.dataSize, dataPatch.data, GL_STATIC_DRAW);
		glVertexAttribPointer(i, dataPatch.attrLen, GL_FLOAT, GL_FALSE, dataPatch.attrLen * sizeof(float), (void*)(0 * sizeof(float)));
		glEnableVertexAttribArray(i);
		if(divisors.size()!=0 && divisors[i] > 0)
			glVertexAttribDivisor(i, divisors[i]);
	}
	//按照没有索引的情况设置 pointNum
	this->pointNum = vec_VBO_DataPatch[0].dataSize / (sizeof(float) * vec_VBO_DataPatch[0].attrLen);
	hasIndex = false;
	isValid = true;
}

VAO::VAO(const std::vector<VBO_DataPatch> & vec_VBO_DataPatch, size_t const * index, size_t indexSize, const std::vector<size_t> & divisors)
	: VAO(vec_VBO_DataPatch, divisors){
	if (IsValid()) {
		pointNum = indexSize / sizeof(size_t);
		hasIndex = true;
		isValid = GenBindEBO(index, indexSize);
	}
}

bool VAO::GenBindEBO(size_t const * index, size_t indexSize) {
	size_t EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, index, GL_STATIC_DRAW);
	return true;
}

size_t VAO::GetID() const {
	return ID;
}

size_t VAO::GetAttrNum() const {
	return ID;
}

bool VAO::IsValid() const {
	return isValid;
}

bool VAO::Use() const {
	if (!IsValid())
		return false;

	glBindVertexArray(ID);
	return true;
}

bool VAO::Draw(const Shader & shader) const {
	if (!Use() || !shader.Use())
		return false;
	if (hasIndex)
		glDrawElements(GL_TRIANGLES, pointNum, GL_UNSIGNED_INT, NULL);
	else
		glDrawArrays(GL_TRIANGLES, 0, pointNum);

	return true;
}