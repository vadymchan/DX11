#pragma once


#include "../Ray Casting/dependencies/glm/glm/glm.hpp"
#include "../Ray Casting/dependencies/glm/glm/gtc/quaternion.hpp"
#include "../Ray Casting/dependencies/glm/glm/gtx/quaternion.hpp"




namespace general::engine
{

	struct Transform // We can build a transformation matrix from this
	{
		glm::quat rotation = glm::quat(1, 0, 0, 0);
		glm::vec3 position;
		glm::vec3 scale;
	};

	//in radians
	struct Angles
	{
		Angles(float pitch, float yaw, float roll)
		{
			this->pitch = pitch;
			this->yaw = yaw;
			this->roll = roll;
		}
		float pitch;
		float yaw;
		float roll;
	};

	class Camera
	{
		Transform transform;

		glm::mat4 proj;
		glm::mat4x4 view;
		glm::vec3 cameraPos;

		bool m_updatedBasis = false;
		bool m_updatedMatrices = false;



	public:

		const glm::vec3& right() 	const { return glm::vec3(view[0][0], view[1][0], view[2][0]); }
		const glm::vec3& up() 		const { return glm::vec3(view[0][1], view[1][1], view[2][1]); }
		const glm::vec3& forward() 	const { return glm::vec3(view[0][2], view[1][2], view[2][2]); }
		const glm::vec3& position()	const { return cameraPos; }


		void setPerspective(float fov, float aspect, float zNear, float zFar)
		{
			proj = glm::perspectiveLH(fov, aspect, zNear, zFar);
		}

		void setView(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& cameraUp)
		{
			cameraPos = position;
			view = glm::lookAtLH(position, position + direction, cameraUp);
		}

		glm::mat4 getIPV()
		{
			if (!m_updatedMatrices)
			{
				updateMatrices();
			}
			return glm::inverse(proj * view);
		}

		void setWorldOffset(const glm::vec3& offset)
		{
			m_updatedMatrices = false;
			view[3][0] = offset.x;
			view[3][1] = offset.y;
			view[3][2] = offset.z;
			cameraPos = offset;

		}

		void addWorldOffset(const glm::vec3& offset)
		{
			m_updatedMatrices = false;

			view[3][0] += offset.x;
			view[3][1] += offset.y;
			view[3][2] += offset.z;
			cameraPos += offset;

		}

		void addRelativeOffset(const glm::vec3& offset) // relative to camera axis
		{
			updateBasis();

			m_updatedMatrices = false;
			view = glm::translate(view, -offset);
			cameraPos += offset;


		}

		void setWorldAngles(const Angles& angles)
		{
			m_updatedBasis = false;
			m_updatedMatrices = false;

			transform.rotation = glm::angleAxis(angles.roll, glm::vec3{ 0.f, 0.f, 1.f });
			transform.rotation *= glm::angleAxis(angles.pitch, glm::vec3{ 1.f, 0.f, 0.f });
			transform.rotation *= glm::angleAxis(angles.yaw, glm::vec3{ 0.f, 1.f, 0.f });

			transform.rotation = glm::normalize(transform.rotation);
		}

		void addWorldAngles(const Angles& angles)
		{
			m_updatedBasis = false;
			m_updatedMatrices = false;
			transform.rotation *= glm::angleAxis(angles.roll, glm::vec3{ 0.f, 0.f, 1.f });
			transform.rotation *= glm::angleAxis(angles.pitch, glm::vec3{ 1.f, 0.f, 0.f });
			transform.rotation *= glm::angleAxis(angles.yaw, glm::vec3{ 0.f, 1.f, 0.f });

			transform.rotation = glm::normalize(transform.rotation);
		}

		void addRelativeAngles(const Angles& angles)
		{
			m_updatedBasis = false;
			m_updatedMatrices = false;

			transform.rotation *= glm::angleAxis(angles.roll, glm::vec3{ 0.f, 0.f, 1.f });
			transform.rotation *= glm::angleAxis(angles.pitch, glm::vec3{ 1.f, 0.f, 0.f });
			transform.rotation *= glm::angleAxis(angles.yaw, glm::vec3{ 0.f, 1.f, 0.f });

			transform.rotation = glm::normalize(transform.rotation);
		}

		void updateBasis()
		{
			if (m_updatedBasis) return;
			m_updatedBasis = true;
			view = glm::transpose(glm::toMat4(transform.rotation)) * view;

		}

		void updateMatrices()
		{
			if (m_updatedMatrices) return;
			m_updatedMatrices = true;
			updateBasis();

		}
	};



}

