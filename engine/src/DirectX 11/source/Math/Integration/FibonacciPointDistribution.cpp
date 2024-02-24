#include "FibonacciPointDistribution.h"

/**
* Compute sine and cosine of the given angle.
*
* @param theta: The angle in radians.
* @param sine: Output parameter for the sine of the angle.
* @param cosine: Output parameter for the cosine of the angle.
*/
void engine::sincos(float theta, float& sine, float& cosine)
{
	sine = std::sin(theta);
	cosine = std::cos(theta);
}

/**
* Generate a random point on a hemisphere.
*
* @param NdotV: Output parameter for the dot product of the normal and view vectors.
* @param i: The current sample index.
* @param N: The total number of samples.
* @return An array containing the 3D coordinates of the random point on the hemisphere.
*/
std::array<float, 3> engine::randomHemisphere(float& NdotV, float i, float N)
{
	const float GOLDEN_RATIO = (1.0 + sqrt(5.0)) / 2.0;
	float theta = 2.0 * PI * i / GOLDEN_RATIO;
	float phiCos = NdotV = 1.0 - (i + 0.5) / N;
	float phiSin = sqrt(1.0 - phiCos * phiCos);
	float thetaCos, thetaSin;
	sincos(theta, thetaSin, thetaCos);
	return std::array<float, 3>{thetaCos* phiSin, thetaSin* phiSin, phiCos};
}

/**
* Compute the integral over a hemisphere using Monte Carlo sampling.
*
* @param numSamples: The number of random samples to generate on the hemisphere.
* @return The computed integral value.
*/
double engine::hemisphereIntegral(int numSamples)
{
	double hemisphereIntegral = 0;

	// Generate random points on the hemisphere and accumulate their contribution to the integral.
	for (int i = 0; i < numSamples; ++i) {
		float NdotV{};
		auto point = randomHemisphere(NdotV, static_cast<float>(i), static_cast<float>(numSamples));
		hemisphereIntegral += NdotV / numSamples;
	}

	// Scale the integral by the surface area of the hemisphere (2*PI).
	hemisphereIntegral *= 2.0f * PI;

	return hemisphereIntegral;
}
