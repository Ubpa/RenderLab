#ifndef _CPPUTIL_ENGINE_SCENE_SOBJ_SAVER_H_
#define _CPPUTIL_ENGINE_SCENE_SOBJ_SAVER_H_

#include <CppUtil/Basic/Visitor.h>
#include <3rdParty/tinyxml2.h>
#include <CppUtil/Basic/UGM/Transform.h>

#include <CppUtil/Basic/UGM/Point.h>
#include <CppUtil/Basic/UGM/Vector.h>
#include <CppUtil/Basic/UGM/Normal.h>
#include <CppUtil/Basic/UGM/RGB.h>
#include <CppUtil/Basic/UGM/RGBA.h>

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

		class CmptLight;
		class AreaLight;
		class PointLight;
		class DirectionalLight;
		class SpotLight;
		class InfiniteAreaLight;
		class SphereLight;

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

			void Init(const std::string & path);

		private:
			void Visit(Basic::PtrC<Basic::Image> img);
			void Visit(Basic::Ptr<Basic::Image> img);

			void Visit(Basic::Ptr<SObj> sobj);
			void Visit(Basic::Ptr<CmptCamera> camera);

			void Visit(Basic::Ptr<CmptGeometry> geometry);
			void Visit(Basic::Ptr<Sphere> sphere);
			void Visit(Basic::Ptr<Plane> plane);
			void Visit(Basic::Ptr<TriMesh> mesh);

			void Visit(Basic::Ptr<CmptLight> light);
			void Visit(Basic::Ptr<AreaLight> areaLight);
			void Visit(Basic::Ptr<PointLight> pointLight);
			void Visit(Basic::Ptr<DirectionalLight> directionalLight);
			void Visit(Basic::Ptr<SpotLight> spotLight);
			void Visit(Basic::Ptr<InfiniteAreaLight> infiniteAreaLight);
			void Visit(Basic::Ptr<SphereLight> sphereLight);

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
			
			template<int N, typename T>
			void NewEle(const char * const name, const Val<N, T> & val) {
				std::stringstream ss;
				for (int i = 0; i < N - 1; i++)
					ss << val[i] << " ";
				ss << val[N-1];
				NewEle(name, ss.str());
			}

			template<int N, typename T>
			void NewEle(const char * const name, const Point<N, T> & val) {
				NewEle(name, static_cast<Val<N, T>>(val));
			}

			template<int N, typename T>
			void NewEle(const char * const name, const Vector<N, T> & val) {
				NewEle(name, static_cast<Val<N, T>>(val));
			}

			template<typename T>
			void NewEle(const char * const name, const Normal<T> & val) {
				NewEle(name, static_cast<Val<3, T>>(val));
			}

			template<typename T>
			void NewEle(const char * const name, const RGB<T> & val) {
				NewEle(name, static_cast<Val<3, T>>(val));
			}

			template<typename T>
			void NewEle(const char * const name, const RGBA<T> & val) {
				NewEle(name, static_cast<Val<4, T>>(val));
			}

			void Member(tinyxml2::XMLElement * parent, const std::function<void()> & func);

			void NewEle(const char * const name, const std::function<void()> & func) {
				Member(NewEle(name), func);
			}

			void NewEle(const char * const name, Basic::PtrC<Basic::Image> img);

		private:
			tinyxml2::XMLDocument doc;
			std::map<Basic::Ptr<SObj>, tinyxml2::XMLElement *> sobj2ele;
			std::vector<tinyxml2::XMLElement *> parentEleStack;
			std::string path;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_SCENE_SOBJ_SAVER_H_
