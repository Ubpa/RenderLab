#pragma once

#include <OpenGL/Texture.h>

#include <vector>

namespace Ubpa {

	class FBO {
	public:
		enum ENUM_TYPE
		{
			ENUM_TYPE_INVALID,
			ENUM_TYPE_BASIC,
			ENUM_TYPE_RGBF1_DEPTH,
			ENUM_TYPE_RGBF2_DEPTH,
			ENUM_TYPE_RGBF3_DEPTH,
			ENUM_TYPE_MSAA,
			ENUM_TYPE_COLOR,
			ENUM_TYPE_RED,
			ENUM_TYPE_COLOR_FLOAT,
			ENUM_TYPE_DEPTH,
			ENUM_TYPE_CUBE_DEPTH,
			ENUM_TYPE_GBUFFER,
			ENUM_TYPE_RAYTRACING,
			ENUM_TYPE_RTX,
			ENUM_TYPE_DYNAMIC_COLOR,
		};
		enum ENUM_PASS_TYPE {
			ENUM_PASS_COLOR,
			ENUM_PASS_DEPTH,
		};

		FBO();
		FBO(unsigned width, unsigned height, ENUM_TYPE type = ENUM_TYPE_BASIC);
		FBO(unsigned width, unsigned height, const std::vector<unsigned>& dimVecForGBuffer);
		bool PassTo(const FBO& fbo, ENUM_PASS_TYPE passType = ENUM_PASS_COLOR) const;
		bool PassTo(unsigned fboID, unsigned width, unsigned height, ENUM_PASS_TYPE passType = ENUM_PASS_COLOR) const;

		bool Use() const;
		void Resize(unsigned width, unsigned height);
		static void UseDefault();
		static unsigned DefaultBuffer;
		unsigned GetID() const;
		const Texture& GetColorTexture(unsigned idx) const;
		const Texture& GetDepthTexture() const;
		bool IsValid() const;

		enum class TexTarget {
			TEXTURE_2D,
			TEXTURE_CUBE_MAP_POSITIVE_X,
			TEXTURE_CUBE_MAP_NEGATIVE_X,
			TEXTURE_CUBE_MAP_POSITIVE_Y,
			TEXTURE_CUBE_MAP_NEGATIVE_Y,
			TEXTURE_CUBE_MAP_POSITIVE_Z,
			TEXTURE_CUBE_MAP_NEGATIVE_Z,
		};
		bool SetColor(const Texture& tex, TexTarget textarget, int mip = 0);
	private:
		bool GenFBO_BASIC(unsigned width, unsigned height);
		bool GenFBO_RGBF_DEPTH(unsigned width, unsigned height, unsigned colorBufferNum = 1);
		bool GenFBO_MSAA(unsigned width, unsigned height);
		bool GenFBO_COLOR(unsigned width, unsigned height, bool isFloat = false);
		bool GenFBO_RED(unsigned width, unsigned height);
		bool GenFBO_DEPTH(unsigned width, unsigned height);
		bool GenFBO_CUBE_DEPTH(unsigned width, unsigned height);
		bool GenFBO_GBUFFER(unsigned width, unsigned height);
		bool GenFBO_RAYTRACING(unsigned width, unsigned height);
		bool GenFBO_RTX(unsigned width, unsigned height);
		bool GenFBO_DYNAMIC_COLOR(unsigned width, unsigned height);

		bool IsComplete() const;
		static unsigned PassType2GL(ENUM_PASS_TYPE passType);

		ENUM_TYPE type;
		unsigned ID;
		std::vector<Texture> colorTextures;
		Texture depthTexture;
		unsigned width;
		unsigned height;
		bool isValid;
		std::vector<unsigned> dimVec;
	};
}
