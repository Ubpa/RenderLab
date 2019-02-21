#ifndef _CPPUTIL_ENGINE_SCENE_SOBJ_LOADER_H_
#define _CPPUTIL_ENGINE_SCENE_SOBJ_LOADER_H_

#include <3rdParty/tinyxml2.h>
#include <CppUtil/Basic/Ptr.h>

#include <string>
#include <map>
#include <functional>
#include <type_traits>

namespace CppUtil {
	namespace Engine {
		class SObj;

		class Camera;

		class Geometry;
		class Sphere;
		class Plane;
		class TriMesh;

		class Light;
		class AreaLight;

		class Material;
		class BSDF_CookTorrance;
		class BSDF_Diffuse;
		class BSDF_Emission;
		class BSDF_Glass;
		class BSDF_MetalWorkflow;
		class BSDF_Mirror;

		class Transform;

		class SObjLoader {
		public:
			static Basic::Ptr<SObj> Load(const std::string & path);
		private:
			template<typename ValImplType>
			static Basic::Ptr<ValImplType> Load(tinyxml2::XMLElement * ele, ValImplType * useless = nullptr);

			// 用 pack
			template<typename ValImplType, typename ObjType>
			static void LoadAndBind(tinyxml2::XMLElement * ele, Basic::Ptr<ObjType> obj, ValImplType * useless = nullptr);

		private:
			typedef std::map<std::string, std::function<void(tinyxml2::XMLElement *)> > FuncMap;
			static void LoadChildrenEles(tinyxml2::XMLElement * ele, const FuncMap & funcMap);


			template<typename T>
			static void Reg(FuncMap & funcMap, const char * const text, T & val) {
				funcMap[text] = [&](tinyxml2::XMLElement * ele) {
					val = To<T>(ele->GetText());
				};
			}

			template<typename T>
			static void Reg(FuncMap & funcMap, const char * const text, const std::function<void(const T &)> & func) {
				funcMap[text] = [=](tinyxml2::XMLElement * ele) {
					func(To<T>(ele->GetText()));
				};
			}

			template<typename ValType, typename ObjType, typename RetType>
			static void Reg(FuncMap & funcMap, const char * const text, Basic::Ptr<ObjType> obj, RetType(ObjType::*setVal)(ValType)) {
				funcMap[text] = [=](tinyxml2::XMLElement * ele) {
					((*obj).*setVal)(To<std::remove_cv<std::remove_reference<ValType>::type>::type>(ele->GetText()));
				};
			}
			
			static void Reg(FuncMap & funcMap, const char * const text, const std::function<void(tinyxml2::XMLElement *)> & func) {
				funcMap[text] = [=](tinyxml2::XMLElement * ele) {
					func(ele);
				};
			}

			// 将 ele 视为 ValImpl 载入
			template<typename ValImplType>
			static void Reg(FuncMap & funcMap, const char * const text, ValImplType * useless = nullptr) {
				Reg(funcMap, text, [=](tinyxml2::XMLElement * ele) {
					Load<ValImplType>(ele);
				});
			}

			// 将 ele 视为 ValImpl 载入，并调用 LoadAndBind
			template<typename ValImplType, typename ObjType>
			static void Reg(FuncMap & funcMap, const char * const text, Basic::Ptr<ObjType> obj, ValImplType * useless = nullptr) {
				Reg(funcMap, text, [=](tinyxml2::XMLElement * ele) {
					LoadAndBind<ValImplType>(ele, obj);
				});
			}

			// 将 ele 视为 ValImpl 载入，并调用一个设置函数
			template<typename ValImplType, typename ValType, typename ObjType>
			static void Reg(FuncMap & funcMap, const char * const text, Basic::Ptr<ObjType> obj, void(ObjType::*setVal)(Basic::Ptr<ValType>), ValImplType * useless = nullptr) {
				Reg(funcMap, text, [=](tinyxml2::XMLElement * ele) {
					((*obj).*setVal)(Load<ValImplType>(ele));
				});
			}

			template<typename ObjType, typename ValType>
			struct Pack {
				Pack(Basic::Ptr<ObjType> obj, void(ObjType::*setVal)(Basic::Ptr<ValType>))
					:obj(obj), setVal(setVal) { }

				FuncMap funcMap;
				Basic::Ptr<ObjType> obj;
				void(ObjType::*setVal)(Basic::Ptr<ValType>);
			};

			template<typename ObjType, typename ObjBaseType, typename ValType>
			static Pack<ObjType, ValType> GenPack(Basic::Ptr<ObjType> obj, void(ObjBaseType::*setVal)(Basic::Ptr<ValType>)) {
				return Pack<ObjType, ValType>(obj, setVal);
			}

			template<typename ValImplType, typename ValType, typename ObjType>
			static void Reg(Pack<ObjType, ValType> & pack, const char * const text, ValImplType * useless = nullptr) {
				Reg<ValImplType, ValType, ObjType>(pack.funcMap, text, pack.obj, pack.setVal);
			}

		private:
			template<typename T>
			static T To(const std::string & text, T * useless_arg = nullptr);
		};
	}
}

#endif//!_CPPUTIL_ENGINE_SCENE_SOBJ_SAVER_H_
