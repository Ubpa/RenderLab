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
		class PointLight;

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
			// 将 ele 视为 ValImplType 载入
			template<typename ValImplType>
			static Basic::Ptr<ValImplType> Load(tinyxml2::XMLElement * ele, ValImplType * useless = nullptr);

			// 将 ele 视为 ValImplType 载入，内部要进行绑定，或者将绑定 obj 的任务传到下一级
			template<typename ValImplType, typename ObjType>
			static void LoadAndBind(tinyxml2::XMLElement * ele, Basic::Ptr<ObjType> obj, ValImplType * useless = nullptr);

		private:
			typedef std::map<std::string, std::function<void(tinyxml2::XMLElement *)> > FuncMap;
			// 根据 funcMap 将子 element 载入
			static void LoadChildrenEles(tinyxml2::XMLElement * ele, const FuncMap & funcMap);

			// 将 name == key 的 ele 的 text 以 T 赋值给 val
			template<typename T>
			static void Reg(FuncMap & funcMap, const char * const key, T & val) {
				funcMap[key] = [&](tinyxml2::XMLElement * ele) {
					val = To<T>(ele->GetText());
				};
			}

			// 将 name == key 的 ele 的 text 以 T 做参数调用func
			template<typename T>
			static void Reg(FuncMap & funcMap, const char * const key, const std::function<void(const T &)> & func) {
				funcMap[key] = [=](tinyxml2::XMLElement * ele) {
					func(To<T>(ele->GetText()));
				};
			}

			// 将 name == key 的 ele 的 text 以 T 做参数调用 obj的setVal
			template<typename ValType, typename ObjType, typename RetType>
			static void Reg(FuncMap & funcMap, const char * const key, Basic::Ptr<ObjType> obj, RetType(ObjType::*setVal)(ValType)) {
				funcMap[key] = [=](tinyxml2::XMLElement * ele) {
					((*obj).*setVal)(To<std::remove_cv<std::remove_reference<ValType>::type>::type>(ele->GetText()));
				};
			}
			
			// 将 name == key 的 ele 做参数调用 func
			static void Reg(FuncMap & funcMap, const char * const key, const std::function<void(tinyxml2::XMLElement *)> & func) {
				funcMap[key] = [=](tinyxml2::XMLElement * ele) {
					func(ele);
				};
			}

			// 将 name == key 的 ele 视为 ValImpl，调用 Load 载入
			template<typename ValImplType>
			static void Reg(FuncMap & funcMap, const char * const key, ValImplType * useless = nullptr) {
				Reg(funcMap, key, [=](tinyxml2::XMLElement * ele) {
					Load<ValImplType>(ele);
				});
			}

			// 将 name == key 的 ele 视为 ValImpl，调用 LoadAndBind 载入并绑定
			template<typename ValImplType, typename ObjType>
			static void Reg(FuncMap & funcMap, const char * const key, Basic::Ptr<ObjType> obj, ValImplType * useless = nullptr) {
				Reg(funcMap, key, [=](tinyxml2::XMLElement * ele) {
					LoadAndBind<ValImplType>(ele, obj);
				});
			}

			// 将 ele 视为 ValImpl 载入，并调用一个设置函数
			template<typename ValImplType, typename ValType, typename ObjType>
			static void Reg(FuncMap & funcMap, const char * const key, Basic::Ptr<ObjType> obj, void(ObjType::*setVal)(Basic::Ptr<ValType>), ValImplType * useless = nullptr) {
				Reg(funcMap, key, [=](tinyxml2::XMLElement * ele) {
					((*obj).*setVal)(Load<ValImplType>(ele));
				});
			}

			// 这里要求 ObjBaseType 是 ObjType 的基类
			template<typename ObjType, typename ValType>
			struct Pack {
				template<typename ObjBaseType>
				Pack(Basic::Ptr<ObjType> obj, void(ObjBaseType::*setVal)(Basic::Ptr<ValType>))
					:obj(obj), setVal(setVal) { }

				FuncMap funcMap;
				Basic::Ptr<ObjType> obj;
				void(ObjType::*setVal)(Basic::Ptr<ValType>);
			};

			// 辅助生成 Pack，这样就不需要手动设置类模板类型了
			template<typename ObjType, typename ValType, typename ObjBaseType>
			static Pack<ObjType, ValType> GenPack(Basic::Ptr<ObjType> obj, void(ObjBaseType::*setVal)(Basic::Ptr<ValType>)) {
				return Pack<ObjType, ValType>(obj, setVal);
			}

			// 辅助用函数
			template<typename ValImplType, typename ValType, typename ObjType>
			static void Reg(Pack<ObjType, ValType> & pack, const char * const key, ValImplType * useless = nullptr) {
				Reg<ValImplType, ValType, ObjType>(pack.funcMap, key, pack.obj, pack.setVal);
			}

		private:
			template<typename T>
			static T To(const std::string & key, T * useless_arg = nullptr);
		};
	}
}

#endif//!_CPPUTIL_ENGINE_SCENE_SOBJ_SAVER_H_
