#pragma once

#include "../dependencies/glm-0.9.9.9/glm/mat4x4.hpp"



namespace Engine
{
	class Camera
	{
	public:



		Camera(glm::vec3 position, glm::vec3 cameraDirection, glm::vec3 up, float nearPlane, float farPlane, float fov, float aspectRatio)
			: position(position)
			, direction(glm::normalize(-cameraDirection))
			, cameraRight(glm::normalize(glm::cross(up, direction)))
			, cameraUp(glm::cross(direction, cameraRight))
			, worldUp(up)
			, nearPlane(nearPlane)
			, farPlane(farPlane)
			, fov(fov)
			, aspectRatio(aspectRatio)

		{

		}

		void changePosition(glm::vec3 newPosition)
		{
			position = newPosition;
		}

		void changeDirection(glm::vec3 newDirection)
		{
			direction = newDirection;
		}

		void changeAspectRatio(float value)
		{
			aspectRatio = value;
		}

		glm::vec3 getPosition() const { return position; }
		glm::vec3 getCameraUp() const { return cameraUp; }
		glm::vec3 getCameraRight() const { return cameraRight; }
		glm::vec3 getDirection() const { return direction; }
		glm::vec3 getWorldUp() const { return worldUp; }
		float getFOV() const { return fov; }
		float getNearPlane() const { return nearPlane; }
		float getFarPlane() const { return farPlane; }
		float getAspectRatio() const { return aspectRatio; }



	private:
		glm::vec3 worldUp;
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 cameraRight;
		glm::vec3 cameraUp;



		float nearPlane;
		float farPlane;
		float fov;
		float aspectRatio;

	};

}


