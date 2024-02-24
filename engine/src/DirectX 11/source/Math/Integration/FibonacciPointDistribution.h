#pragma once

#include <iostream>
#include <array>
#include <cmath>

namespace engine
{
	const float PI = 3.14159265358979323846;

	/**
	* Compute sine and cosine of the given angle.
	*
	* @param theta: The angle in radians.
	* @param sine: Output parameter for the sine of the angle.
	* @param cosine: Output parameter for the cosine of the angle.
	*/
	void sincos(float theta, float& sine, float& cosine);

	/**
	 * Generate a random point on a hemisphere.
	 *
	 * @param NdotV: Output parameter for the dot product of the normal and view vectors.
	 * @param i: The current sample index.
	 * @param N: The total number of samples.
	 * @return An array containing the 3D coordinates of the random point on the hemisphere.
	 */
	std::array<float, 3> randomHemisphere(float& NdotV, float i, float N);

	/**
	 * Compute the integral over a hemisphere using Monte Carlo sampling.
	 *
	 * @param numSamples: The number of random samples to generate on the hemisphere.
	 * @return The computed integral value.
	 */
	double hemisphereIntegral(int numSamples);

}


