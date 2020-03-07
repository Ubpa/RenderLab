#include "BVHNode.h"

#include <Engine/Viewer/BVHAccel.h>
#include <Basic/Math.h>

using namespace Ubpa;

using namespace std;

void BVHNode::Build(const unordered_map<Ptr<Shape>, bboxf3> & shape2wbbox, vector<Ptr<Shape>> & shapes) {
	// Build bvh form shapesOffset to shapesOffset + shapesNum

	constexpr int bucketNum = 12;
	constexpr int maxLeafSize = 4;
	constexpr double t_trav = 0.125;

	bboxf3 extentBox;
	for (size_t i = shapesOffset; i < shapesOffset + shapesNum; i++) {
		auto target = shape2wbbox.find(shapes[i]);
		assert(target != shape2wbbox.cend());
		const auto worldBox = target->second;

		extentBox.combine_with(worldBox.center());
		box.combine_with(worldBox);
	}

	if (shapesNum <= maxLeafSize) {
		l = nullptr;
		r = nullptr;
		return;
	}

	// get best partition
	vector<Ptr<Shape>> bestPartition[2];
	double minCost = DBL_MAX;
	for (int dim = 0; dim < 3; dim++) {

		// 1. compute buckets

		vector<vector<Ptr<Shape>>> buckets(bucketNum);
		vector<bboxf3> boxesOfBuckets(bucketNum);
		{// 将 shape 放到 bucket 中，计算好每个 bucket 的 box
			double bucketLen = extentBox.diagonal()[dim] / bucketNum;
			if (bucketLen == 0)
				continue;

			double left = extentBox.minP()[dim];
			for (size_t i = shapesOffset; i < shapesOffset + shapesNum; i++) {
				auto target = shape2wbbox.find(shapes[i]);
				assert(target != shape2wbbox.cend());
				const auto & worldBox = target->second;

				double center = worldBox.center()[dim];
				int bucketID = Math::Clamp(static_cast<int>((center - left) / bucketLen), 0, bucketNum - 1);

				buckets[bucketID].push_back(shapes[i]);
				boxesOfBuckets[bucketID].combine_with(worldBox);
			}
		}

		// 2. accumulate buckets

		vector<bboxf3> leftBox(bucketNum);
		vector<size_t> leftAccNum(bucketNum);
		vector<bboxf3> rightBox(bucketNum);
		vector<size_t> rightAccNum(bucketNum);
		leftAccNum[0] = 0;
		rightAccNum[0] = 0;
		for (int i = 1; i <= bucketNum - 1; i++) {
			leftBox[i] = leftBox[i - 1].combine(boxesOfBuckets[i - 1]);
			leftAccNum[i] = leftAccNum[i - 1] + buckets[i - 1].size();

			rightBox[i] = rightBox[i - 1].combine(boxesOfBuckets[bucketNum - i]);
			rightAccNum[i] = rightAccNum[i - 1] + buckets[bucketNum - i].size();
		}

		// 3. get best partition of dim
		int bestLeftBucketsNum = 0;
		double minCostOfDim = DBL_MAX;
		for (int leftNum = 1; leftNum <= bucketNum - 1; leftNum++) {
			int rightNum = bucketNum - leftNum;

			double leftS = leftBox[leftNum].area();
			double rightS = rightBox[rightNum].area();

			double curCost = t_trav + leftS * leftAccNum[leftNum] + rightS * rightAccNum[rightNum];

			if (curCost < minCostOfDim) {
				bestLeftBucketsNum = leftNum;
				minCostOfDim = curCost;
			}
		}

		// 4. set best partition
		if (minCostOfDim < minCost) {
			bestPartition[0].clear();
			bestPartition[1].clear();

			minCost = minCostOfDim;
			axis = dim;
			for (int i = 0; i < bestLeftBucketsNum; i++) {
				for (auto primitive : buckets[i])
					bestPartition[0].push_back(primitive);
			}
			for (int i = bestLeftBucketsNum; i < bucketNum; i++) {
				for (auto primitive : buckets[i])
					bestPartition[1].push_back(primitive);
			}
		}
	}

	// recursion
	if (bestPartition[0].size() == 0 || bestPartition[1].size() == 0)
		return;

	for (size_t i = 0; i < bestPartition[0].size(); i++)
		shapes[i + shapesOffset] = bestPartition[0][i];
	for (size_t i = 0; i < bestPartition[1].size(); i++)
		shapes[i + shapesOffset + bestPartition[0].size()] = bestPartition[1][i];

	l = BVHNode::New(shape2wbbox, shapes, shapesOffset, bestPartition[0].size());
	r = BVHNode::New(shape2wbbox, shapes, shapesOffset + bestPartition[0].size(), bestPartition[1].size());
}
