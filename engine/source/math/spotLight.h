#pragma once
#include "../../dependencies/glm-0.9.9.9/glm/ext/vector_float3.hpp"

namespace Engine
{
	class SpotLight
	{
	public:
		/// <param name="outerCutoff">outer cutoff of light. The value is angle between light direction and outer edge of spot light</param>
		/// <param name="innerCutoff">inner cutoff of light. The value is angle between light direction and inner edge of spot light </param>
		SpotLight(const Sphere& sphere, const glm::vec3& direction, float outerCutoff, float innerCutoff)
			: sphere{sphere}
			, direction(direction)
			, outerCutoff(outerCutoff)
			, innerCutoff(innerCutoff)
		{
			cosOuterCutoff = glm::cos(outerCutoff);
			cosInnerCutoff = glm::cos(innerCutoff);

		};

		void setDirection(glm::vec3 value) { direction = value; }

		const glm::vec3& getPosition() const {	return sphere.getCenter(); }
		const glm::vec3& getDirection() const { return direction; }
		float getOuterCutoff() const { return outerCutoff; }
		float getInnerCutoff() const { return innerCutoff; }
		float getCosInnerCutoff() const { return cosInnerCutoff; }
		float getCosOuterCutoff() const { return cosOuterCutoff; }
		float getRadius() const { return sphere.getRadius(); }
		Sphere* GetPtrSphere() { return &sphere; }
		bool hit(const ray& r, Intersection& near_t) { return sphere.hit(r, near_t); }

	private:
		Sphere sphere;
		glm::vec3 direction;
		float outerCutoff;
		float innerCutoff;
		float cosOuterCutoff;
		float cosInnerCutoff;
	};
}

