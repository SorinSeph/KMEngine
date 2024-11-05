#pragma once

#include <math.h>
#include <cstdlib>

#define SMALL_NUMBER (1.e-8f)

template <typename T>
static T Square(const T Arg)
{
	return Arg * Arg;
}

template< class T >
static constexpr T Clamp(const T X, const T Min, const T Max)
{
	return (X < Min) ? Min : (X < Max) ? X : Max;
}

static bool IsNearlyEqual(float A, float B, float ErrorTolerance = SMALL_NUMBER)
{
	return std::abs(A - B) <= ErrorTolerance;
}

template <typename T1, typename T2 = T1, typename T3 = T2, typename T4 = T3>
auto FInterpConstantTo(T1 Current, T2 Target, T3 DeltaTime, T4 InterpSpeed)
{
	using RetType = decltype(T1()* T2()* T3()* T4());

	const RetType Dist = Target - Current;

	// If distance is too small, just set the desired location
	if (Square(Dist) < SMALL_NUMBER)
	{
		return static_cast<RetType>(Target);
	}

	const RetType Step = InterpSpeed * DeltaTime;
	return Current + Clamp(Dist, -Step, Step);
}