#pragma once


#ifndef RAY_H
#define RAY_H



#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace engine::RC
{
	

	class ray
	{
	public:

		ray(const glm::vec3& origin, const glm::vec3& direction)
			: orig(origin), dir(glm::normalize(direction))
		{}

		glm::vec3 origin() const { return orig; } // A
		glm::vec3 direction() const { return dir; } // b

		void changeOrigin(glm::vec3 newOrigin) { orig = newOrigin; }
		void changeDirection(glm::vec3 newDirection) { dir = newDirection; }

		glm::vec3 getPointAt(float t) const
		{
			return orig + t * dir; //A + t * b  
		}

		void operator *= (glm::mat4 tMat)
		{
			this->orig = tMat * glm::vec4(orig, 1) ;
			this->dir = tMat * glm::vec4(dir, 0) ;
		}

	private:
		glm::vec3 orig;
		glm::vec3 dir;
	};

	//check for intersection within 3 points (triangle)
	bool hit(const ray& r, float& near_t, const glm::vec3& V1, const glm::vec3& V2, const glm::vec3& V3);

#endif 

}