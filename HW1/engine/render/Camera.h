#pragma once

#include "../dependencies/glm-0.9.9.9/glm/mat4x4.hpp"





class Camera
{
public:

	Camera() {}

	Camera(glm::vec3 position, glm::vec3 cameraDirection, glm::vec3 up, float nearPlane, float farPlane, float fov, float aspectRatio)
		: position(position)
		, direction(glm::normalize(-cameraDirection)) 
		, cameraRight(glm::normalize(glm::cross( up, direction))) 
		, cameraUp(glm::cross(direction, cameraRight )) 
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

	glm::vec3 getPosition()
	{
		return position;
	}

	glm::vec3 getCameraUp()
	{
		return cameraUp;
	}

	glm::vec3 getCameraRight()
	{
		return cameraRight;
	}

	glm::vec3 getDirection()
	{
		return direction;
	}

	float getFOV()
	{
		return fov;
	}

	float getNearPlane()
	{
		return nearPlane;
	}

	float getFarPlane()
	{
		return farPlane;
	}

	float getAspectRatio()
	{
		return aspectRatio;
	}

	glm::mat4x4 Iview;
	glm::mat4x4 Ipers;

private:
	//glm::vec3 up;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 cameraRight;
	glm::vec3 cameraUp;



	float nearPlane;
	float farPlane;
	float fov;
	float aspectRatio;

};

