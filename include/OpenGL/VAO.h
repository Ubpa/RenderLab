#pragma once

#include <vector>

namespace Ubpa {
	class Shader;

	class VAO {
	public:
		struct VBO_DataPatch {
			float const* data;
			unsigned dataSize;
			unsigned attrLen;
		};
		VAO();
		// dataSize is sizeof(data), not elementNum of data
		VAO(float const* data, unsigned dataSize, const std::vector<unsigned>& attrLen);
		VAO(float const* data, unsigned dataSize, const std::vector<unsigned>& attrLen, unsigned const* index, unsigned indexSize);
		VAO(const std::vector<VBO_DataPatch>& vec_VBO_DataPatch, const std::vector<unsigned>& divisors = std::vector<unsigned>());
		VAO(const std::vector<VBO_DataPatch>& vec_VBO_DataPatch, unsigned const* index, unsigned indexSize, const std::vector<unsigned>& divisors = std::vector<unsigned>());

		unsigned GetID() const;
		unsigned GetAttrNum() const;
		bool IsValid() const;
		bool Use() const;
		bool Draw(const Shader& shader) const;
	private:
		bool GenBindEBO(unsigned const* index, unsigned indexSize);
		//------------
		unsigned attrNum;
		unsigned ID;
		unsigned pointNum;
		bool hasIndex;
		bool isValid;

	public:
		static const VAO inValid;
	};
}
