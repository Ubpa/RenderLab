#ifndef _CPPUTIL_ENGINE_SCENE_SOBJ_SAVER_H_
#define _CPPUTIL_ENGINE_SCENE_SOBJ_SAVER_H_

#include <CppUtil/Basic/EleVisitor.h>
#include <3rdParty/tinyxml2.h>

#include <stack>
#include <glm/glm.hpp>
#include <functional>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {
		class SObj;

		class Camera;

		class Geometry;
		class Sphere;
		class Plane;
		class TriMesh;

		class Light;
		class AreaLight;
		class PointLight;

		class Material;
		class BSDF_CookTorrance;
		class BSDF_Diffuse;
		class BSDF_Emission;
		class BSDF_Glass;
		class BSDF_MetalWorkflow;
		class BSDF_Mirror;
		class BSDF_FrostedGlass;

		class Transform;

		class SObjSaver : public Basic::EleVisitor {
			ELEVISITOR_SETUP(SObjSaver)
		public:
			SObjSaver();
			void Init(const std::string & path);

		private:
			void Visit(Basic::CPtr<Basic::Image> img);
			void Visit(Basic::Ptr<Basic::Image> img);

			void Visit(Basic::Ptr<SObj> sobj);
			void Visit(Basic::Ptr<Camera> camera);

			void Visit(Basic::Ptr<Geometry> geometry);
			void Visit(Basic::Ptr<Sphere> sphere);
			void Visit(Basic::Ptr<Plane> plane);
			void Visit(Basic::Ptr<TriMesh> mesh);

			void Visit(Basic::Ptr<Light> light);
			void Visit(Basic::Ptr<AreaLight> areaLight);
			void Visit(Basic::Ptr<PointLight> pointLight);

			void Visit(Basic::Ptr<Material> meterial);
			void Visit(Basic::Ptr<BSDF_CookTorrance> bsdf);
			void Visit(Basic::Ptr<BSDF_Diffuse> bsdf);
			void Visit(Basic::Ptr<BSDF_Emission> bsdf);
			void Visit(Basic::Ptr<BSDF_Glass> bsdf);
			void Visit(Basic::Ptr<BSDF_MetalWorkflow> bsdf);
			void Visit(Basic::Ptr<BSDF_Mirror> bsdf);
			void Visit(Basic::Ptr<BSDF_FrostedGlass> bsdf);

			void Visit(Basic::Ptr<Transform> transform);

		private:
			tinyxml2::XMLText * NewText(const std::string & text) {
				return doc.NewText(text.c_str());
			}

			// int, long, long long
			// unsigned, unsigned long, unsigned long long
			// float, double, long double
			template<typename numT>
			tinyxml2::XMLText * NewText(numT val) {
				return NewText(std::to_string(val));
			}

			tinyxml2::XMLElement * NewEle(tinyxml2::XMLElement * parent, const char * const name) {
				auto ele = doc.NewElement(name);
				parent->InsertEndChild(ele);
				return ele;
			}

			tinyxml2::XMLElement * NewEle(const char * const name) {
				return NewEle(parentEleStack.back(), name);
			}

			void NewEle(tinyxml2::XMLElement * parent, const char * const name, const std::string & text) {
				auto ele = NewEle(parent, name);
				ele->InsertEndChild(NewText(text));
			}

			void NewEle(const char * const name, const std::string & text) {
				NewEle(parentEleStack.back(), name, text);
			}

			void NewEle(const char * const name, float val) {
				NewEle(parentEleStack.back(), name, std::to_string(val));
			}

			void NewEle(const char * const name, int val) {
				NewEle(parentEleStack.back(), name, std::to_string(val));
			}

			void NewEle(const char * const name, const glm::vec3 & vec);

			void Member(tinyxml2::XMLElement * parent, const std::function<void()> & func);

			void NewEle(const char * const name, const std::function<void()> & func) {
				Member(NewEle(name), func);
			}

		private:
			tinyxml2::XMLDocument doc;
			std::map<Basic::Ptr<SObj>, tinyxml2::XMLElement *> sobj2ele;
			std::vector<tinyxml2::XMLElement *> parentEleStack;
			std::string path;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_SCENE_SOBJ_SAVER_H_
