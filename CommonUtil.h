#pragma once

#include <random>

#include "Config.h"

namespace Adoter
{
	template<typename T, typename ...Args>
	std::unique_ptr<T> make_unique(Args&& ...args)
	{
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}

#define DEBUG_ASSERT(expr) \
	auto debug = ConfigInstance.GetBool("DebugModel", true); \
		assert(debug && expr); \

}

class CommonUtil
{
public:
	static int32_t Random(int32_t min, int32_t max) //[min, max]
	{
		static std::default_random_engine _generator;
		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(_generator);
	}

	template<class IT, class WEIGHT>
	static int RandomByWeight(const IT& begin, const IT& end, WEIGHT w)
	{
		int sum = 0;
		for(auto it = begin; it != end; ++it) sum += w(*it);
		if(sum <= 0) return -1;
		int rnd = Random(0, sum - 1);
		size_t index = 0;
		int weight = 0;
		for(auto it = begin; it != end; ++it, ++index)
		{
			weight = w(*it);
			if(rnd < weight)
				return index;
			rnd -= weight;
		}
		return -1;
	}
	
	template<class IT, class WEIGHT>
	static IT RandomItorByWeight(const IT& begin, const IT& end, WEIGHT w)
	{
		int sum = 0;
		for(auto it = begin; it != end; ++it) sum += w(*it);
		if(sum <= 0) return end;
		int rnd = Random(0, sum - 1);
		int weight = 0;
		for(auto it = begin; it != end; ++it)
		{
			weight = w(*it);
			if(rnd < weight)
				return it;
			rnd -= weight;
		}
		return end;
	}
};
