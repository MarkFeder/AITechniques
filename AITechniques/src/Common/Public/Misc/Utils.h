#pragma once

#include <math.h>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include <cassert>
#include <iomanip>

// A few useful constants
const int MaxInt = (std::numeric_limits<int>::max)();
const double MaxDouble = (std::numeric_limits<double>::max)();
const double MinDouble = (std::numeric_limits<double>::min)();
const float MaxFloat = (std::numeric_limits<float>::max)();
const float MinFloat = (std::numeric_limits<float>::min)();

const double Pi = 3.14159;
const double TwoPi = Pi * 2;
const double HalfPi = Pi / 2;
const double QuarterPi = Pi / 4;

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

// Returns true is the third parameter is in the range described by
// the first two
inline bool InRange(double start, double end, double val)
{
	if (start < end)
	{
		return ((val > start) && (val < end));
	}
	else
	{
		return ((val < start) && (val > end));
	}
}

// Returns true if two double values are equal or not
inline bool IsEqual(double a, double b) { return (fabs(a - b) < 1E-12); }
// Returns true if two real numbers are equal or not
inline bool IsEqual(float a, float b) { return (fabs(a - b) < 1E-12); }

// Returns the maximum of two values
template <class T>
T Maximum(const T& v1, const T& v2)
{
	return v1 > v2 ? v1 : v2;
}

// Some Random number functions

// Returns a random integer between x and y
inline int RandInt(int x, int y) { return rand() % (y - x + 1) + x; }

// Returns a random double between zero and 1
inline double RandFloat() { return ((rand()) / (RAND_MAX + 1.0f)); }
