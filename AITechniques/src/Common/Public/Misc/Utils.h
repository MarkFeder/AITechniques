#pragma once

#include <math.h>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include <cassert>
#include <iomanip>

//----------------------------------------------------------------------------
//  A few useful constants.
//----------------------------------------------------------------------------

const int MaxInt = (std::numeric_limits<int>::max)();
const double MaxDouble = (std::numeric_limits<double>::max)();
const double MinDouble = (std::numeric_limits<double>::min)();
const float MaxFloat = (std::numeric_limits<float>::max)();
const float MinFloat = (std::numeric_limits<float>::min)();

const double Pi = 3.14159;
const double TwoPi = Pi * 2;
const double HalfPi = Pi / 2;
const double QuarterPi = Pi / 4;

//----------------------------------------------------------------------------
//  Some random number functions.
//----------------------------------------------------------------------------

// Returns a random integer between x and y
inline int RandInt(int x, int y)
{
	return rand() % (y - x + 1) + x;
}

// Returns a random double between zero and 1
inline double RandFloat() 
{
	return ((rand()) / (RAND_MAX + 1.0f)); 
}

// Returns a random bool
inline bool RandomBool()
{
	return (RandInt(0, 1)) ? true : false;
}

// Returns a random double in the range -1 < n < 1
inline double RandomClamped()
{
	return RandFloat() - RandFloat();
}

//----------------------------------------------------------------------------
//  Some handy little functions.
//----------------------------------------------------------------------------

// Returns true is the third parameter is in the range described by
// the first two
inline bool InRange(double start, double end, double val)
{
	return (start < end) ? ((val > start) && (val < end)) : ((val < start) && (val > end));
}

// Returns true if two double values are equal or not
inline bool IsEqual(double a, double b)
{
	return (fabs(a - b) < 1E-12);
}

// Returns true if two real numbers are equal or not
inline bool IsEqual(float a, float b)
{
	return (fabs(a - b) < 1E-12);
}

// Returns the maximum of two values
template <class T>
T Maximum(const T& v1, const T& v2)
{
	return v1 > v2 ? v1 : v2;
}

// Returns an average from a vector
template <class T>
inline double Average(const std::vector<T>& v)
{
	double average = 0.0;

	for (unsigned int i = 0; i < v.size(); ++i)
	{
		average += (double)v[i];
	}

	return average / (double)v.size();
}

// Deletes the internal contents of a STL container
template <class T>
inline void DeleteSTLContainer(T& container)
{
	for (T::iterator it = container.begin(); it != container.end(); ++it)
	{
		delete *it;
		*it = nullptr;
	}
}

// Deletes the internal contents of a STL map
template <class T>
inline void DeleteSTLMap(T& map)
{
	for (T::iterator it = map.begin(); it != map.end(); ++it)
	{
		delete it->second;
		it->second = nullptr;
	}
}

inline double Sigmoid(double input, double response = 1.0)
{
	return (1.0 / (1.0 + exp(-input / response)));
}

// Returns true if the value is a NaN
template <typename T>
inline bool IsNaN(T val)
{
	return val != val;
}

// Converts input degrees to radians
inline double DegsToRads(double degs)
{
	return TwoPi * (degs / 360.0);
}

// Returns true if the parameter is equal to zero
inline bool IsZero(double val)
{
	return ((-MinDouble < val) && (val < MinDouble));
}
