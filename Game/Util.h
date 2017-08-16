#pragma once

#include <stdexcept>

class Util
{
public:
	template<typename T>
	static const bool AssertEquals(const T& lhs, const T& rhs, bool throwException = false)
	{
		bool equals = (lhs == rhs);

		if (throwException && !equals)
		{
			throw std::invalid_argument(lhs + " ! equal to " + rhs);
		}

		return equals;
	}

	template<class T>
	static const bool AssertLessThan(const T& lhs, const T& rhs, bool including, bool throwException = false)
	{
		bool isLessThan;

		if (including)	isLessThan = (lhs <= rhs);
		else			isLessThan = (lhs <  rhs);

		if (throwException && !isLessThan)
		{
			throw std::invalid_argument(lhs + " ! less than " + rhs);
		}
		
		return isLessThan;
	}

	template<class T>
	static const bool AssertGreaterThan(const T& lhs, const T& rhs, bool including, bool throwException = false)
	{
		bool isGreaterThan;

		if (including)	isGreaterThan = (lhs >= rhs);
		else			isGreaterThan = (lhs > rhs);

		if (throwException && !isGreaterThan) 
		{
			throw std::invalid_argument(lhs + " ! greater than " + rhs);
		}

		return isGreaterThan;
	}

	static const int ToPowerOfTwo(int x)
	{
		x--;

		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;

		x++;

		return x;
	}
};

