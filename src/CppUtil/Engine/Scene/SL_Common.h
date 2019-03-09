#ifndef _CPPUTIL_ENGINE_SCENE_SL_COMMON_H_
#define _CPPUTIL_ENGINE_SCENE_SL_COMMON_H_

#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/AllComponents.h>

#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/TriMesh.h>

#include <CppUtil/Engine/AreaLight.h>
#include <CppUtil/Engine/PointLight.h>

#include <CppUtil/Engine/AllBSDFs.h>

#include <CppUtil/Basic/Image.h>

#include <functional>

namespace str {
	// ------------- SObj

	namespace SObj {
		const char * const type = "SObj";

		const char * const name = "name";
		const char * const components = "components";
		const char * const children = "children";
	}

	// component

	// ------------- Camera

	namespace Camera {
		const char * const type = "Camera";

		const char * const fov = "fov";
		const char * const ar = "ar";
		const char * const nearPlane = "nearPlane";
		const char * const farPlane = "farPlane";
	}

	// ------------- Geometry

	namespace Geometry {
		const char * const type = "Geometry";
		const char * const primitive = "primitive";
	}

	namespace Sphere {
		const char * const type = "Sphere";

		const char * const center = "center";
		const char * const radius = "radius";
	}

	namespace Plane {
		const char * const type = "Plane";
	}

	namespace TriMesh {
		const char * const type = "TriMesh";

		namespace ENUM_TYPE {
			const char * const INVALID = "invalid";
			const char * const CODE = "code";
			const char * const CUBE = "cube";
			const char * const SPHERE = "sphere";
			const char * const PLANE = "plane";
			const char * const FILE = "file";
		}
	}

	// ------------- Light

	namespace Light {
		const char * const type = "Light";
		
		const char * const lightBase = "lightBase";
	}

	namespace AreaLight {
		const char * const type = "Area_Light";

		const char * const color = "color";
		const char * const intensity = "intensity";
		const char * const width = "width";
		const char * const height = "height";
	}

	namespace PointLight {
		const char * const type = "Point_Light";

		const char * const color = "color";
		const char * const intensity = "intensity";
		const char * const linear = "linear";
		const char * const quadratic = "quadratic";
	}

	// ------------- Material

	namespace Material {
		const char * const type = "Material";

		const char * const materialBase = "materialBase";
	}

	// ------------- Transform

	namespace Transform {
		const char * const type = "Transform";

		const char * const Position = "Position";
		const char * const Rotation = "Rotation";
		const char * const Scale = "Scale";
	}

	namespace BSDF_CookTorrance {
		const char * const type = "BSDF_CookTorrance";

		const char * const ior = "ior";
		const char * const roughness = "roughness";
		const char * const refletance = "refletance";
		const char * const albedo = "albedo";
	}

	namespace BSDF_Diffuse {
		const char * const type = "BSDF_Diffuse";

		const char * const albedoColor = "albedoColor";
		const char * const albedoTexture = "albedoTexture";
	}

	namespace BSDF_Emission {
		const char * const type = "BSDF_Emission";

		const char * const color = "color";
		const char * const intensity = "intensity";
	}

	namespace BSDF_Glass {
		const char * const type = "BSDF_Glass";

		const char * const ior = "ior";
		const char * const transmittance = "transmittance";
		const char * const reflectance = "reflectance";
	}

	namespace Image {
		const char * const path = "path";
	}

	namespace BSDF_MetalWorkflow {
		const char * const type = "BSDF_MetalWorkflow";

		const char * const albedoColor = "albedoColor";
		const char * const albedoTexture = "albedoTexture";
		const char * const metallicFactor = "metallicFactor";
		const char * const metallicTexture = "metallicTexture";
		const char * const roughnessFactor = "roughnessFactor";
		const char * const roughnessTexture = "roughnessTexture";
		const char * const aoTexture = "aoTexture";
		const char * const normalTexture = "normalTexture";
	}

	namespace BSDF_FrostedGlass {
		const char * const type = "BSDF_FrostedGlass";

		const char * const IOR = "IOR";
		const char * const colorFactor = "colorFactor";
		const char * const colorTexture = "colorTexture";
		const char * const roughnessFactor = "roughnessFactor";
		const char * const roughnessTexture = "roughnessTexture";
		const char * const aoTexture = "aoTexture";
		const char * const normalTexture = "normalTexture";
	}

	namespace BSDF_Mirror {
		const char * const type = "BSDF_Mirror";

		const char * const reflectance = "reflectance";
	}
}

#endif//!_CPPUTIL_ENGINE_SCENE_SL_COMMON_H_
