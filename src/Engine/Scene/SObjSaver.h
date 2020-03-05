#ifndef _CPPUTIL_ENGINE_SCENE_SOBJ_SAVER_H_
#define _CPPUTIL_ENGINE_SCENE_SOBJ_SAVER_H_

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

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {
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

		class SObjSaver : public Basic::Visitor {
		public:
			SObjSaver();

		public:
			static const Basic::Ptr<SObjSaver> New() {
				return Basic::New<SObjSaver>();
			}

			void Init(const std::string& path);

		private:
			void Visit(Basic::PtrC<Basic::Image> img);
			void Visit(Basic::Ptr<Basic::Image> img);

			void Visit(Basic::Ptr<SObj> sobj);
			void Visit(Basic::Ptr<CmptCamera> camera);

			void Visit(Basic::Ptr<CmptGeometry> geometry);
			void Visit(Basic::Ptr<Sphere> sphere);
			void Visit(Basic::Ptr<Plane> plane);
			void Visit(Basic::Ptr<TriMesh> mesh);
			void Visit(Basic::Ptr<Capsule> capsule);
			void Visit(Basic::Ptr<Disk> disk);

			void Visit(Basic::Ptr<CmptLight> light);
			void Visit(Basic::Ptr<AreaLight> areaLight);
			void Visit(Basic::Ptr<PointLight> pointLight);
			void Visit(Basic::Ptr<DirectionalLight> directionalLight);
			void Visit(Basic::Ptr<SpotLight> spotLight);
			void Visit(Basic::Ptr<InfiniteAreaLight> infiniteAreaLight);
			void Visit(Basic::Ptr<SphereLight> sphereLight);
			void Visit(Basic::Ptr<DiskLight> diskLight);
			void Visit(Basic::Ptr<CapsuleLight> diskLight);

			void Visit(Basic::Ptr<CmptMaterial> meterial);
			void Visit(Basic::Ptr<BSDF_CookTorrance> bsdf);
			void Visit(Basic::Ptr<BSDF_Diffuse> bsdf);
			void Visit(Basic::Ptr<BSDF_Emission> bsdf);
			void Visit(Basic::Ptr<BSDF_Glass> bsdf);
			void Visit(Basic::Ptr<BSDF_MetalWorkflow> bsdf);
			void Visit(Basic::Ptr<BSDF_Mirror> bsdf);
			void Visit(Basic::Ptr<BSDF_FrostedGlass> bsdf);
			void Visit(Basic::Ptr<Gooch> gooch);
			void Visit(Basic::Ptr<BSDF_Frostbite> bsdf);

			void Visit(Basic::Ptr<CmptTransform> transform);

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
			void NewEle(const char* const name, const Ubpa::val<T, N>& val) {
				std::stringstream ss;
				for (int i = 0; i < N - 1; i++)
					ss << val[i] << " ";
				ss << val[N - 1];
				NewEle(name, ss.str());
			}

			template<size_t N, typename T>
			void NewEle(const char* const name, const Ubpa::point<T, N>& val) {
				NewEle(name, val.cast_to<Ubpa::val<T, N>>());
			}

			template<size_t N, typename T>
			void NewEle(const char* const name, const Ubpa::vec<T, N>& val) {
				NewEle(name, val.cast_to<Ubpa::val<T, N>>());
			}

			template<size_t N, typename T>
			void NewEle(const char* const name, const Ubpa::scale<T, N>& val) {
				NewEle(name, val.cast_to<Ubpa::val<T, N>>());
			}

			template<typename T>
			void NewEle(const char* const name, const Ubpa::normal<T>& val) {
				NewEle(name, val.cast_to<Ubpa::val<T, 3>>());
			}

			template<typename T>
			void NewEle(const char* const name, const Ubpa::euler<T>& val) {
				NewEle(name, val.cast_to<Ubpa::val<T, 3>>());
			}

			template<typename T>
			void NewEle(const char* const name, const Ubpa::rgb<T>& val) {
				NewEle(name, val.cast_to<Ubpa::val<T, 3>>());
			}

			template<typename T>
			void NewEle(const char* const name, const Ubpa::quat<T>& val) {
				NewEle(name, val.cast_to<Ubpa::val<T, 4>>());
			}

			template<typename T>
			void NewEle(const char* const name, const Ubpa::rgba<T>& val) {
				NewEle(name, val.cast_to<Ubpa::val<T, 4>>());
			}

			void Member(tinyxml2::XMLElement* parent, const std::function<void()>& func);

			void NewEle(const char* const name, const std::function<void()>& func) {
				Member(NewEle(name), func);
			}

			void NewEle(const char* const name, Basic::PtrC<Basic::Image> img);

		private:
			tinyxml2::XMLDocument doc;
			std::map<Basic::Ptr<SObj>, tinyxml2::XMLElement*> sobj2ele;
			std::vector<tinyxml2::XMLElement*> parentEleStack;
			std::string path;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_SCENE_SOBJ_SAVER_H_
