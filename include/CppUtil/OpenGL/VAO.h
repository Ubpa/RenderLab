#ifndef _OPENGL_VAO_VAO_H_
#define _OPENGL_VAO_VAO_H_

typedef unsigned int uint;

#include <vector>

namespace CppUtil {
	namespace OpenGL {
		class Shader;

		class VAO {
		public:
			struct VBO_DataPatch {
				float const * data;
				uint dataSize;
				uint attrLen;
			};
			VAO();
			// dataSize is sizeof(data), not elementNum of data
			VAO(float const * data, uint dataSize, const std::vector<uint> & attrLen);
			VAO(float const * data, uint dataSize, const std::vector<uint> & attrLen, uint const * index, uint indexSize);
			VAO(const std::vector<VBO_DataPatch> & vec_VBO_DataPatch, const std::vector<uint> & divisors = std::vector<uint>());
			VAO(const std::vector<VBO_DataPatch> & vec_VBO_DataPatch, uint const * index, uint indexSize, const std::vector<uint> & divisors = std::vector<uint>());

			uint GetID() const;
			uint GetAttrNum() const;
			bool IsValid() const;
			bool Use() const;
			bool Draw(const Shader & shader) const;
		private:
			bool GenBindEBO(uint const * index, uint indexSize);
			//------------
			uint attrNum;
			uint ID;
			uint pointNum;
			bool hasIndex;
			bool isValid;

		public:
			static const VAO inValid;
		};
	}
}

#endif // !_OPENGL_VAO_VAO_H_
