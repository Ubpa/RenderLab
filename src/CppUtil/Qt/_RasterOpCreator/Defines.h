#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <CppUtil/OpenGL/CommonDefine.h>
#include <CppUtil/Basic/Point3.h>
#include <CppUtil/Basic/RGB.h>

namespace Define {
	//------------ shader

	const std::string str_ShaderPrefix = "/data/shaders/CppUtil/Qt/RasterOpCreator/";

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

	const CppUtil::Point3 data_LightPositions[lightNum] = {
		CppUtil::Point3(0.0f, 0.5f,  1.5f),
		CppUtil::Point3(-4.0f, 0.5f, -3.0f),
		CppUtil::Point3(3.0f, 0.5f,  1.0f),
		CppUtil::Point3(-.8f,  2.4f, -1.0f)
	};

	const CppUtil::RGBf data_LightColors[lightNum] = {
		CppUtil::RGBf(2.0f, 2.0f, 2.0f),
		CppUtil::RGBf(1.5f, 0.0f, 0.0f),
		CppUtil::RGBf(0.0f, 0.0f, 1.5f),
		CppUtil::RGBf(0.0f, 1.5f, 0.0f)
	};
}

#endif // !_DEFINES_H_


