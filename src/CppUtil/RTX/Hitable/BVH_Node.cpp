#include <CppUtil/RTX/BVH_Node.h>

#include <CppUtil/RTX/Ray.h>

#include <CppUtil/Basic/Math.h>

#include <algorithm>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

BVH_Node::BVH_Node(Material::CPtr material)
	: box(AABB::InValid), Hitable(material) { }

BVH_Node::BVH_Node(vector<Hitable::CPtr> & hitables, Material::CPtr material)
	: box(AABB::InValid), Hitable(material){
	if (hitables.size() == 0)
		return;

	remove_if(hitables.begin(), hitables.end(), [](Hitable::CPtr hitable)->bool { return hitable->GetBoundingBox().IsValid(); });
	Build(hitables.cbegin(), hitables.cend());
}

BVH_Node::BVH_Node(const vector<Hitable::CPtr>::const_iterator begin, const vector<Hitable::CPtr>::const_iterator end, Material::CPtr material)
	: box(AABB::InValid), Hitable(material) { Build(begin, end); }

void BVH_Node::Build(const vector<Hitable::CPtr>::const_iterator begin, const vector<Hitable::CPtr>::const_iterator end){
	size_t num = end - begin;

	if (num == 1) {
		left = *begin;
		right = NULL;
		box = left->GetBoundingBox();
		return;
	}
	
	if (num == 2) {
		left = *begin;
		right = *(begin+1);
		box = left->GetBoundingBox() + right->GetBoundingBox();
		return;
	}
	
	constexpr size_t bucketNum = 8;

	for (auto nd = begin; nd != end; nd++)
		box.Expand((*nd)->GetBoundingBox());

	// get best partition
	vector<Hitable::CPtr> bestPartition[2];
	double minCost = DBL_MAX;
	for (size_t dim = 0; dim < 3; dim++) {
		// 1. compute buckets
		double bucketLen = box.GetExtent()[dim] / bucketNum;
		double left = box.GetMinP()[dim];
		vector<vector<Hitable::CPtr>> buckets(bucketNum);
		vector<AABB> boxesOfBuckets(bucketNum);
		for (size_t i = 0; i < num; i++) {
			Hitable::CPtr hitable = *(begin + i);
			AABB bb = hitable->GetBoundingBox();
			double center = bb.GetCenter()[dim];
			size_t bucketID = (center - left) / bucketLen;
			buckets[bucketID].push_back(hitable);
			boxesOfBuckets[bucketID].Expand(bb);
		}

		// 2. accumulate buckets
		vector<AABB> leftBox(bucketNum);
		vector<size_t> leftAccNum(bucketNum);
		leftAccNum[0] = 0;
		vector<AABB> rightBox(bucketNum);
		vector<size_t> rightAccNum(bucketNum);
		rightAccNum[0] = 0;
		for (size_t i = 1; i <= bucketNum - 1; i++) {
			leftBox[i] = leftBox[i - 1];
			leftBox[i].Expand(boxesOfBuckets[i - 1]);
			leftAccNum[i] = leftAccNum[i - 1] + buckets[i - 1].size();
			rightBox[i] = rightBox[i - 1];
			rightBox[i].Expand(boxesOfBuckets[bucketNum - i]);
			rightAccNum[i] = rightAccNum[i - 1] + buckets[bucketNum - i].size();
		}

		// 3. get best partition of dim
		size_t bestLeftNum = 0;
		double minCostDim = DBL_MAX;
		for (size_t leftNum = 1; leftNum <= bucketNum - 1; leftNum++) {
			size_t rightNum = bucketNum - leftNum;
			double leftS = leftBox[leftNum].GetSurfaceArea();
			double rightS = rightBox[rightNum].GetSurfaceArea();
			double costDim = leftS * leftAccNum[leftNum] + rightS * rightAccNum[rightNum];
			if (costDim < minCostDim) {
				bestLeftNum = leftNum;
				minCostDim = costDim;
			}
		}

		// 4. set best partition
		if (minCostDim < minCost) {
			bestPartition[0].clear();
			bestPartition[1].clear();

			minCost = minCostDim;
			for (size_t i = 0; i < bestLeftNum; i++) {
				for (auto primitive : buckets[i])
					bestPartition[0].push_back(primitive);
			}
			for (size_t i = bestLeftNum; i < bucketNum; i++) {
				for (auto primitive : buckets[i])
					bestPartition[1].push_back(primitive);
			}
		}
	}

	// recursion
	if (bestPartition[0].size() == num || bestPartition[1].size() == num){
		size_t idx = bestPartition[0].size() == num ? 0 : 1;
		size_t leftNum = num / 2;
		left = ToCPtr(new BVH_Node(bestPartition[idx].cbegin(), bestPartition[idx].cbegin() + leftNum));
		right = ToCPtr(new BVH_Node(bestPartition[idx].cbegin() + leftNum, bestPartition[idx].cend()));
	}
	else {
		left = ToCPtr(new BVH_Node(bestPartition[0].cbegin(), bestPartition[0].cend()));
		right = ToCPtr(new BVH_Node(bestPartition[1].cbegin(), bestPartition[1].cend()));
	}
}

HitRst BVH_Node::RayIn(Ray::Ptr & ray) const {
	float t1, t2, t3, t4;
	bool leftBoxHit = left != NULL ? left->GetBoundingBox().Hit(ray, t1, t2) : false;
	bool rightBoxHit = right != NULL ? right->GetBoundingBox().Hit(ray, t3, t4) : false;

	HitRst hitRst;
	if (leftBoxHit) {
		if (rightBoxHit) {
			Hitable::CPtr first = (t1 <= t3) ? left : right;
			Hitable::CPtr second = (t1 <= t3) ? right : left;
			hitRst = first->RayIn(ray);
			if (ray->GetTMax() > t3) {
				HitRst secondHitRst = second->RayIn(ray);
				if (secondHitRst.hit)
					hitRst = secondHitRst;
			}
		}
		else
			hitRst = left->RayIn(ray);
	}
	else if (rightBoxHit)
		hitRst = right->RayIn(ray);
	else
		return HitRst::InValid;

	if (GetMat() != NULL && hitRst.isMatCoverable) {
		hitRst.material = GetMat();
		hitRst.isMatCoverable = IsMatCoverable();
	}

	return hitRst;
}
