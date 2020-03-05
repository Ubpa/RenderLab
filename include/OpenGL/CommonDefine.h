#pragma once

#include <string>

namespace Define{
	const size_t val_windowWidth = 1920;
	const size_t val_windowHeight = 1080;
	const float val_RatioWH = (float)val_windowWidth / (float)val_windowHeight;
	const float PI = 3.1415926f;
	
	const std::string str_MainCamera = "MainCamera";
	const std::string str_DeltaTime = "DeltaTime";
	const std::string str_MainConfig = "MainConfig";
	const std::string str_RootPath = "RootPath";
	
	const std::string config_CameraMoveSpeed = "CameraMoveSpeed";
	const std::string config_CameraRotateSensitivity = "CameraRotateSensitivity";

	//------------ textures
	const std::string texture_Prefix = "/data/textures/";
	const std::string texture_wood = texture_Prefix + "wood.jpg";
	const std::string texture_earth = texture_Prefix + "earth.jpg";
	const std::string texture_box = texture_Prefix + "box.png";

	//------------ common shader
	const std::string str_ShadersCommon_Prefix = "/data/shaders/Common/";
	const std::string str_VsPostfix = ".vs";
	const std::string str_GsPostfix = ".gs";
	const std::string str_FsPostfix = ".fs";

	const std::string str_Basic_P3N3T2T3_vs = str_ShadersCommon_Prefix + "basic_P3N3T2T3" + str_VsPostfix;
	const std::string str_Basic_P3N3T2T3_offset_vs = str_ShadersCommon_Prefix + "basic_P3N3T2T3_offset" + str_VsPostfix;
	const std::string str_Basic_P3N3T2_invN_vs = str_ShadersCommon_Prefix + "basic_P3N3T2_invN" + str_VsPostfix;
	const std::string str_Basic_P3N3T2_vs = str_ShadersCommon_Prefix + "basic_P3N3T2" + str_VsPostfix;
	const std::string str_Basic_P3N3_invN_vs = str_ShadersCommon_Prefix + "basic_P3N3_invN" + str_VsPostfix;
	const std::string str_Basic_P3N3_vs = str_ShadersCommon_Prefix + "basic_P3N3" + str_VsPostfix;
	const std::string str_Basic_P3T2_vs = str_ShadersCommon_Prefix + "basic_P3T2" + str_VsPostfix;
	const std::string str_Basic_P2T2_vs = str_ShadersCommon_Prefix + "basic_P2T2" + str_VsPostfix;
	const std::string str_Basic_P3_vs = str_ShadersCommon_Prefix + "basic_P3" + str_VsPostfix;

	const std::string str_Basic_T2_fs = str_ShadersCommon_Prefix + "basic_T2" + str_FsPostfix;
	const std::string str_Basic_fs = str_ShadersCommon_Prefix + "basic" + str_FsPostfix;

	const std::string str_ImgShow_vs = str_Basic_P3T2_vs;
	const std::string str_ImgShow_fs = str_Basic_T2_fs;

	const std::string str_ColorShow_vs = str_Basic_P3_vs;
	const std::string str_ColorShow_fs = str_Basic_fs;

	const std::string str_Screen_vs = str_Basic_P2T2_vs;
	const std::string str_Screen_fs = str_Basic_T2_fs;

	const std::string str_ppBlur_vs = str_Basic_P2T2_vs;
	const std::string str_ppBlur_fs = str_ShadersCommon_Prefix + "ppBlur" + str_FsPostfix;

	const std::string str_Interpolation_vs = str_Basic_P2T2_vs;
	const std::string str_Interpolation_fs = str_ShadersCommon_Prefix + "Interpolation" + str_FsPostfix;

	const std::string str_PostProcess_vs = str_Basic_P2T2_vs;
	const std::string str_PostProcess_fs = str_ShadersCommon_Prefix + "postProcess" + str_FsPostfix;

	const std::string str_empty_fs = str_ShadersCommon_Prefix + "empty" + str_FsPostfix;

	const std::string str_MVP_P3_vs = str_ShadersCommon_Prefix + "MVP_P3" + str_VsPostfix;

	const float data_ScreenVertices[] = { // vertex attributes for a quad that fills the entire screen in normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	const float data_Flip_ScreenVertices[] = { // vertex attributes for a quad that fills the entire screen in normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 1.0f,

		-1.0f,  1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 1.0f,
		 1.0f,  1.0f,  1.0f, 0.0f
	};
}
