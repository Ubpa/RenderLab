#pragma once

#include <Basic/Element.h>

#include <string>
#include <vector>
#include <mutex>
#include <map>

namespace CppUtil {
	namespace Engine {
		class SObj;

		class CmptCamera;
		class CmptLight;

		class InfiniteAreaLight;

		class Scene : public Basic::Element {
		public:
			Scene(Basic::Ptr<SObj> root, const std::string & name = "")
				: root(root), name(name) { }

		public:
			static const Basic::Ptr<Scene> New(Basic::Ptr<SObj> root, const std::string & name = "") {
				return Basic::New<Scene>(root, name);
			}

		protected:
			virtual ~Scene() = default;

		public:
			const Basic::Ptr<SObj> GetRoot() const { return root; }

			const Basic::Ptr<CmptCamera> GetCmptCamera() const;

			const std::vector<Basic::Ptr<CmptLight>> GetCmptLights() const;

			const Basic::Ptr<InfiniteAreaLight> GetInfiniteAreaLight() const;

			// sobjs have no same name
			// ID >= 1
			bool GenID();

			// ID >= 1, return 0 if error
			int GetID(Basic::Ptr<SObj> sobj) const;
			// ID >= 1, return "" if error
			const std::string GetName(int ID) const;

			void SetWriteLock(bool isLock);

		public:
			std::string name;

		private:
			Basic::Ptr<SObj> root;
			std::map<std::string, int> name2ID;
			std::map<int, std::string> ID2name;

			mutable std::mutex writeLock;
		};
	}
}
