#pragma once

#include <Basic/HeapObj.h>

#include <string>
#include <vector>
#include <mutex>
#include <map>

namespace Ubpa {
	class SObj;

	class CmptCamera;
	class CmptLight;

	class InfiniteAreaLight;

	class Scene : public HeapObj {
	public:
		Scene(Ptr<SObj> root, const std::string& name = "")
			: root(root), name(name) { }

	public:
		static const Ptr<Scene> New(Ptr<SObj> root, const std::string& name = "") {
			return Ubpa::New<Scene>(root, name);
		}

	protected:
		virtual ~Scene() = default;

	public:
		const Ptr<SObj> GetRoot() const { return root; }

		const Ptr<CmptCamera> GetCmptCamera() const;

		const std::vector<Ptr<CmptLight>> GetCmptLights() const;

		const Ptr<InfiniteAreaLight> GetInfiniteAreaLight() const;

		// sobjs have no same name
		// ID >= 1
		bool GenID();

		// ID >= 1, return 0 if error
		int GetID(Ptr<SObj> sobj) const;
		// ID >= 1, return "" if error
		const std::string GetName(int ID) const;

		void SetWriteLock(bool isLock);

	public:
		std::string name;

	private:
		Ptr<SObj> root;
		std::map<std::string, int> name2ID;
		std::map<int, std::string> ID2name;

		mutable std::mutex writeLock;
	};
}
