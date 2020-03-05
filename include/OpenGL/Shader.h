#pragma once

#include <UGM/val.h>
#include <UGM/transform.h>

#include <string>

namespace Ubpa {
	class Shader {
		enum class CompileType {
			VERTEX, GEOMETRY, FRAGMENT, PROGRAM
		};

	public:
		// constructor generates the shader on the fly
		Shader();
		Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "");

		unsigned GetID() const;
		bool IsValid() const;

		// activate the shader
		bool Use() const;

		// utility uniform functions
		void SetBool(const std::string& name, bool value) const;
		void SetInt(const std::string& name, int value) const;
		void SetTex(const std::string& name, int value) const {
			SetInt(name, value);
		}
		void SetFloat(const std::string& name, float value) const;
		void SetFloats(const std::string& name, unsigned n, const float* values) const;
		void SetVecf2(const std::string& name, float value0, float value1) const;
		void SetVecf2(const std::string& name, const valf2& v) const {
			SetVecf2(name, v[0], v[1]);
		}
		void SetVecf3(const std::string& name, float value0, float value1, float value2) const;
		void SetVecf3(const std::string& name, const valf3& v) const {
			SetVecf3(name, v[0], v[1], v[2]);
		}
		void SetVecf3s(const std::string& name, unsigned n, const float* data) const;
		void SetVec4f(const std::string& name, float value0, float value1, float value2, float value3) const;
		void SetVec4f(const std::string& name, const valf4& v) const {
			SetVec4f(name, v[0], v[1], v[2], v[3]);
		}
		void SetMatf4(const std::string& name, const float* matValue) const;
		void SetMatf4(const std::string& name, const matf4& mat) const {
			SetMatf4(name, mat.data());
		}

		//------------
		void UniformBlockBind(const std::string& name, unsigned bindPoint) const;

	public:
		static const Shader InValid;

	private:
		// include
		static const std::string Process(const std::string& path);

	private:
		// utility function for checking shader compilation/linking errors.
		int CheckCompileErrors(unsigned shader, CompileType type);
		//------------
		unsigned ID;
		bool valid;
	};
}
