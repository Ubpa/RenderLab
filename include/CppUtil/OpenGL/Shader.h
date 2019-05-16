#ifndef _OPENGL_SHADER_SHADER_H_
#define _OPENGL_SHADER_SHADER_H_

typedef unsigned int uint;

#include <CppUtil/Basic/UGM/Val2.h>
#include <CppUtil/Basic/UGM/Val3.h>
#include <CppUtil/Basic/UGM/Val4.h>
#include <CppUtil/Basic/UGM/Transform.h>

#include <string>
#include <enum.h>

namespace CppUtil {
	namespace OpenGL {
		BETTER_ENUM(CompileType, int, VERTEX, GEOMETRY, FRAGMENT, PROGRAM);

		class Shader {
		public:
			// constructor generates the shader on the fly
			Shader();
			Shader(const std::string &  vertexPath, const std::string & fragmentPath, const std::string & geometryPath = "");

			uint GetID() const;
			bool IsValid() const;

			// activate the shader
			bool Use() const;

			// utility uniform functions
			void SetBool(const std::string &name, bool value) const;
			void SetInt(const std::string &name, int value) const;
			void SetTex(const std::string &name, int value) const {
				SetInt(name, value);
			}
			void SetFloat(const std::string &name, float value) const;
			void SetFloats(const std::string &name, uint n, const float * values) const;
			void SetVec2f(const std::string &name, float value0, float value1) const;
			void SetVec2f(const std::string &name, const Val2 & v) const {
				SetVec2f(name, v[0], v[1]);
			}
			void SetVec3f(const std::string &name, float value0, float value1, float value2) const;
			void SetVec3f(const std::string &name, const Val3 & v) const {
				SetVec3f(name, v[0], v[1], v[2]);
			}
			void SetVec3fs(const std::string &name, uint n, const float * data) const;
			void SetVec4f(const std::string &name, float value0, float value1, float value2, float value3) const;
			void SetVec4f(const std::string &name, const Val4 & v) const {
				SetVec4f(name, v[0], v[1], v[2], v[3]);
			}
			void SetMat4f(const std::string &name, const float * matValue) const;
			void SetMat4f(const std::string &name, const Mat4f & mat) const {
				SetMat4f(name, mat.Data());
			}
			void SetMat4f(const std::string &name, const Transform & tsfm) const {
				SetMat4f(name, tsfm.GetMatrix());
			}

			//------------
			void UniformBlockBind(const std::string &name, uint bindPoint) const;
		private:
			// utility function for checking shader compilation/linking errors.
			int CheckCompileErrors(uint shader, CompileType type);
			//------------
			uint ID;
			bool valid;
		};
	}
}

#endif // !_OPENGL_SHADER_SHADER_H_
