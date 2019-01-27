#ifndef _VAO_H_
#define _VAO_H_

#include <vector>

namespace CppUtil {
	namespace OpenGL {
		class Shader;

		class VAO {
		public:
			struct VBO_DataPatch {
				float const * data;
				size_t dataSize;
				size_t attrLen;
			};

			VAO(float const * data, size_t dataSize, const std::vector<size_t> & attrLen);
			VAO(float const * data, size_t dataSize, const std::vector<size_t> & attrLen, size_t const * index, size_t indexSize);
			VAO(const std::vector<VBO_DataPatch> & vec_VBO_DataPatch, const std::vector<size_t> & divisors = std::vector<size_t>());
			VAO(const std::vector<VBO_DataPatch> & vec_VBO_DataPatch, size_t const * index, size_t indexSize, const std::vector<size_t> & divisors = std::vector<size_t>());

			size_t GetID() const;
			size_t GetAttrNum() const;
			bool IsValid() const;
			bool Use() const;
			bool Draw(const Shader & shader) const;
		private:
			bool GenBindEBO(size_t const * index, size_t indexSize);
			//------------
			size_t attrNum;
			size_t ID;
			size_t pointNum;
			bool hasIndex;
			bool isValid;
		};
	}
}

#endif // !_VAO_H_
