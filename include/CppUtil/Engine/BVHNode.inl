template<typename T, typename HolderT>
void CppUtil::Engine::BVHNode<T, HolderT>::Build(size_t maxLeafSize) {
	// Build bvh form start to start + range

	constexpr int bucketNum = 8;

	for (size_t i = start; i < start + range; i++)
		bb.UnionWith(GetBBox(objs[i]));

	if (range < maxLeafSize) {
		l = nullptr;
		r = nullptr;
		return;
	}

	// get best partition
	std::vector<T::Ptr> bestPartition[2];
	double minCost = DBL_MAX;
	for (int dim = 0; dim < 3; dim++) {
		// 1. compute buckets
		double bucketLen = bb.Diagonal()[dim] / bucketNum;
		double left = bb.minP[dim];
		std::vector<std::vector<T::Ptr>> buckets(bucketNum);
		std::vector<BBoxf> boxesOfBuckets(bucketNum);
		for (int i = 0; i < range; i++) {
			BBoxf box = GetBBox(objs[i + start]);
			double center = box.Center()[dim];
			int bucketID = min(static_cast<int>((center - left) / bucketLen), bucketNum - 1);
			buckets[bucketID].push_back(objs[i + start]);
			boxesOfBuckets[bucketID].UnionWith(box);
		}

		// 2. accumulate buckets
		std::vector<BBoxf> leftBox(bucketNum);
		std::vector<size_t> leftAccNum(bucketNum);
		leftAccNum[0] = 0;
		std::vector<BBoxf> rightBox(bucketNum);
		std::vector<size_t> rightAccNum(bucketNum);
		rightAccNum[0] = 0;
		for (int i = 1; i <= bucketNum - 1; i++) {
			leftBox[i] = leftBox[i - 1];
			leftBox[i].UnionWith(boxesOfBuckets[i - 1]);
			leftAccNum[i] = leftAccNum[i - 1] + buckets[i - 1].size();
			rightBox[i] = rightBox[i - 1];
			rightBox[i].UnionWith(boxesOfBuckets[bucketNum - i]);
			rightAccNum[i] = rightAccNum[i - 1] + buckets[bucketNum - i].size();
		}

		// 3. get best partition of dim
		int bestLeftNum = 0;
		double minCostDim = DBL_MAX;
		for (int leftNum = 1; leftNum <= bucketNum - 1; leftNum++) {
			int rightNum = bucketNum - leftNum;
			double leftS = leftBox[leftNum].SurfaceArea();
			double rightS = rightBox[rightNum].SurfaceArea();
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
			for (int i = 0; i < bestLeftNum; i++) {
				for (auto primitive : buckets[i])
					bestPartition[0].push_back(primitive);
			}
			for (int i = bestLeftNum; i < bucketNum; i++) {
				for (auto primitive : buckets[i])
					bestPartition[1].push_back(primitive);
			}
		}
	}

	// recursion
	if (bestPartition[0].size() == range || bestPartition[1].size() == range) {
		size_t leftNum = range / 2;
		l = ToPtr(new BVHNode(holder, objs, start, leftNum, maxLeafSize));
		r = ToPtr(new BVHNode(holder, objs, start + leftNum, range - leftNum, maxLeafSize));
	}
	else {
		for (size_t i = 0; i < bestPartition[0].size(); i++)
			objs[i + start] = bestPartition[0][i];
		for (size_t i = 0; i < bestPartition[1].size(); i++)
			objs[i + start + bestPartition[0].size()] = bestPartition[1][i];

		l = ToPtr(new BVHNode(holder, objs, start, bestPartition[0].size(), maxLeafSize));
		r = ToPtr(new BVHNode(holder, objs, start + bestPartition[0].size(), bestPartition[1].size(), maxLeafSize));
	}
}
