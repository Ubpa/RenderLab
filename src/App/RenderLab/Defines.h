#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <CppUtil/OpenGL/CommonDefine.h>
#include <glm/vec3.hpp>

namespace Define {
	//------------ shader

	const std::string str_ShaderPrefix = "/data/shaders/App/RenderLab/";

	const std::string str_BloomObj = "bloomObj";
	const std::string str_BloomObj_vs = str_Basic_P3N3T2_vs;
	const std::string str_BloomObj_fs = str_ShaderPrefix + str_BloomObj + str_FsPostfix;

	const std::string str_BloomLight = "bloomLight";
	const std::string str_BloomLight_vs = str_Basic_P3N3T2_vs;
	const std::string str_BloomLight_fs = str_ShaderPrefix + str_BloomLight + str_FsPostfix;

	const std::string str_Blur = "blur";
	const std::string str_Blur_vs = str_Basic_P2T2_vs;
	const std::string str_Blur_fs = str_ShaderPrefix + str_Blur + str_FsPostfix;

	const std::string str_Blend = "blend";
	const std::string str_Blend_vs = str_Basic_P2T2_vs;
	const std::string str_Blend_fs = str_ShaderPrefix + str_Blend + str_FsPostfix;


	//------------ data
	const size_t lightNum = 4;

	const glm::vec3 data_LightPositions[lightNum] = {
		glm::vec3(0.0f, 0.5f,  1.5f),
		glm::vec3(-4.0f, 0.5f, -3.0f),
		glm::vec3(3.0f, 0.5f,  1.0f),
		glm::vec3(-.8f,  2.4f, -1.0f)
	};

	const glm::vec3 data_LightColors[lightNum] = {
		glm::vec3(2.0f, 2.0f, 2.0f),
		glm::vec3(1.5f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.5f),
		glm::vec3(0.0f, 1.5f, 0.0f)
	};
}

#endif // !_DEFINES_H_


