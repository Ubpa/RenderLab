#pragma once

#include <Basic/Visitor.h>
#include <3rdParty/tinyxml2.h>
#include <UGM/transform.h>

#include <UGM/point.h>
#include <UGM/vec.h>
#include <UGM/normal.h>
#include <UGM/rgb.h>
#include <UGM/rgba.h>

#include <stack>
#include <functional>
#include <sstream>

namespace Ubpa {
	class Image;

	class SObj;

	class CmptCamera;

	class CmptGeometry;
	class Sphere;
	class Plane;
	class TriMesh;
	class Capsule;
	class Disk;

	class CmptLight;
	class AreaLight;
	class PointLight;
	class DirectionalLight;
	class SpotLight;
	class InfiniteAreaLight;
	class SphereLight;
	class DiskLight;
	class CapsuleLight;

	class CmptMaterial;
	class BSDF_CookTorrance;
	class BSDF_Diffuse;
	class BSDF_Emission;
	class BSDF_Glass;
	class BSDF_MetalWorkflow;
	class BSDF_Mirror;
	class BSDF_FrostedGlass;
	class Gooch;
	class BSDF_Frostbite;

	class CmptTransform;

	class SObjSaver : public Visitor {
	public:
		SObjSaver();

	public:
		static const Ptr<SObjSaver> New() {
			return Ubpa::New<SObjSaver>();
		}

		void Init(const std::string& path);

	private:
		void Visit(PtrC<Image> img);
		void Visit(Ptr<Image> img);

		void Visit(Ptr<SObj> sobj);
		void Visit(Ptr<CmptCamera> camera);

		void Visit(Ptr<CmptGeometry> geometry);
		void Visit(Ptr<Sphere> sphere);
		void Visit(Ptr<Plane> plane);
		void Visit(Ptr<TriMesh> mesh);
		void Visit(Ptr<Capsule> capsule);
		void Visit(Ptr<Disk> disk);

		void Visit(Ptr<CmptLight> light);
		void Visit(Ptr<AreaLight> areaLight);
		void Visit(Ptr<PointLight> pointLight);
		void Visit(Ptr<DirectionalLight> directionalLight);
		void Visit(Ptr<SpotLight> spotLight);
		void Visit(Ptr<InfiniteAreaLight> infiniteAreaLight);
		void Visit(Ptr<SphereLight> sphereLight);
		void Visit(Ptr<DiskLight> diskLight);
		void Visit(Ptr<CapsuleLight> diskLight);

		void Visit(Ptr<CmptMaterial> meterial);
		void Visit(Ptr<BSDF_CookTorrance> bsdf);
		void Visit(Ptr<BSDF_Diffuse> bsdf);
		void Visit(Ptr<BSDF_Emission> bsdf);
		void Visit(Ptr<BSDF_Glass> bsdf);
		void Visit(Ptr<BSDF_MetalWorkflow> bsdf);
		void Visit(Ptr<BSDF_Mirror> bsdf);
		void Visit(Ptr<BSDF_FrostedGlass> bsdf);
		void Visit(Ptr<Gooch> gooch);
		void Visit(Ptr<BSDF_Frostbite> bsdf);

		void Visit(Ptr<CmptTransform> transform);

	private:
		tinyxml2::XMLText* NewText(const char* text) {
			return NewText(std::string(text));
		}
		tinyxml2::XMLText* NewText(const std::string& text) {
			return doc.NewText(text.c_str());
		}

		// int, long, long long
		// unsigned, unsigned long, unsigned long long
		// float, double, long double
		template<typename numT>
		tinyxml2::XMLText* NewText(numT val) {
			return NewText(std::to_string(val));
		}

		tinyxml2::XMLElement* NewEle(tinyxml2::XMLElement* parent, const char* const name) {
			auto ele = doc.NewElement(name);
			parent->InsertEndChild(ele);
			return ele;
		}

		tinyxml2::XMLElement* NewEle(const char* const name) {
			return NewEle(parentEleStack.back(), name);
		}

		void NewEle(tinyxml2::XMLElement* parent, const char* const name, const std::string& text) {
			auto ele = NewEle(parent, name);
			ele->InsertEndChild(NewText(text));
		}

		void NewEle(const char* const name, const std::string& text) {
			NewEle(parentEleStack.back(), name, text);
		}

		void NewEle(const char* const name, float val) {
			NewEle(parentEleStack.back(), name, std::to_string(val));
		}

		void NewEle(const char* const name, int val) {
			NewEle(parentEleStack.back(), name, std::to_string(val));
		}

		template<size_t N, typename T>
		void NewEle(const char* const name, const val<T, N>& v) {
			std::stringstream ss;
			for (int i = 0; i < N - 1; i++)
				ss << v[i] << " ";
			ss << v[N - 1];
			NewEle(name, ss.str());
		}

		template<size_t N, typename T>
		void NewEle(const char* const name, const point<T, N>& v) {
			NewEle(name, v.cast_to<val<T, N>>());
		}

		template<size_t N, typename T>
		void NewEle(const char* const name, const vec<T, N>& v) {
			NewEle(name, v.cast_to<val<T, N>>());
		}

		template<size_t N, typename T>
		void NewEle(const char* const name, const scale<T, N>& v) {
			NewEle(name, v.cast_to<val<T, N>>());
		}

		template<typename T>
		void NewEle(const char* const name, const normal<T>& v) {
			NewEle(name, v.cast_to<val<T, 3>>());
		}

		template<typename T>
		void NewEle(const char* const name, const euler<T>& v) {
			NewEle(name, v.cast_to<val<T, 3>>());
		}

		template<typename T>
		void NewEle(const char* const name, const rgb<T>& v) {
			NewEle(name, v.cast_to<val<T, 3>>());
		}

		template<typename T>
		void NewEle(const char* const name, const quat<T>& v) {
			NewEle(name, v.cast_to<val<T, 4>>());
		}

		template<typename T>
		void NewEle(const char* const name, const rgba<T>& v) {
			NewEle(name, v.cast_to<val<T, 4>>());
		}

		void Member(tinyxml2::XMLElement* parent, const std::function<void()>& func);

		void NewEle(const char* const name, const std::function<void()>& func) {
			Member(NewEle(name), func);
		}

		void NewEle(const char* const name, PtrC<Image> img);

	private:
		tinyxml2::XMLDocument doc;
		std::map<Ptr<SObj>, tinyxml2::XMLElement*> sobj2ele;
		std::vector<tinyxml2::XMLElement*> parentEleStack;
		std::string path;
	};
}
