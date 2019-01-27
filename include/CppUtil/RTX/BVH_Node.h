#ifndef _BVH_NODE_H_
#define _BVH_NODE_H_

#include <CppUtil/RTX/Hitable.h>
#include <vector>

namespace RTX {
	class BVH_Node : public Hitable{
		HITABLE_SETUP(BVH_Node)
	public:
		BVH_Node(Material::CPtr material = NULL);
		BVH_Node(std::vector<Hitable::CPtr> & hitables, Material::CPtr material = NULL);
		BVH_Node(const std::vector<Hitable::CPtr>::const_iterator begin, const std::vector<Hitable::CPtr>::const_iterator end, Material::CPtr material = NULL);

		virtual HitRst RayIn(CppUtil::Basic::Ptr<Ray> & ray) const;
		virtual const AABB GetBoundingBox() const { return box; }
		const Hitable::CPtr GetLeft() const { return left; }
		const Hitable::CPtr GetRight() const { return right; }
	protected:
		void Build(const std::vector<Hitable::CPtr>::const_iterator begin, const std::vector<Hitable::CPtr>::const_iterator end);

		Hitable::CPtr left;
		Hitable::CPtr right;
		AABB box;
	};
}

#endif // !_BVH_NODE_H_
