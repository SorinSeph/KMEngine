#pragma once

#include <vector>

class CPerlinNoise
{
public:
	CPerlinNoise();
	double Noise(double x, double y, double z);

private:
	std::vector<int> Permutation;
	double Fade(double t);
	double Lerp(double t, double a, double b);
	double Gradient(int hash, double x, double y, double z);
};