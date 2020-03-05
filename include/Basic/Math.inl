#pragma once

namespace Ubpa {
	namespace Math {
		template <typename T>
		T Mean(const std::vector<T>& data) {
			if (data.size() == 0)
				return static_cast<T>(0);

			T sum = static_cast<T>(0);
			for (size_t i = 0; i < data.size(); i++)
				sum += data[i];

			return sum / data.size();
		}

		template<typename T>
		T Variance(const std::vector<T>& data) {
			if (data.size() <= 1)
				return static_cast<T>(0);

			T mean = Mean(data);
			T sum = static_cast<T>(0);
			for (size_t i = 0; i < data.size(); i++)
				sum += pow(data[i] - mean, 2);

			return sum / (data.size() - 1);
		}

		template<typename T>
		inline void Permute(std::vector<T> data) {
			for (size_t i = data.size() - 1; i > 0; i--) {
				size_t target = Rand_UI() % i;
				std::swap(data[i], data[target]);
			}
		}

		template<typename T>
		inline T min(const std::vector<T>& val) {
			if (val.empty())
				return static_cast<T>(0);

			T rst = val[0];
			for (size_t i = 1; i < val.size(); i++)
				rst = std::min(rst, val[i]);

			return rst;
		}

		template<typename T>
		inline T max(const std::vector<T>& val) {
			if (val.empty())
				return static_cast<T>(0);

			T rst = val[0];
			for (size_t i = 1; i < val.size(); i++)
				rst = std::max(rst, val[i]);

			return rst;
		}
	}
}
