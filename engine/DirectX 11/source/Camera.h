#pragma once

#include "D3D/D3D.h"





namespace engine::DX
{
	
	struct Transform
	{
		DirectX::SimpleMath::Quaternion rotation;
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector3 scale;
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

		DirectX::SimpleMath::Matrix proj;
		DirectX::SimpleMath::Matrix view;
		DirectX::SimpleMath::Vector3 cameraPos;

		bool basisUpdated = false;
		bool matricesUpdated = false;
		bool bufferUpdated = false;


	public:

		struct ViewProjectionMatrix
		{
			//DirectX::SimpleMath::Matrix view;
			//DirectX::SimpleMath::Matrix proj;
			DirectX::SimpleMath::Matrix viewProj;
		};

		ConstantBuffer<ViewProjectionMatrix> cameraBuffer;

		void initBuffer(UINT registerNumber, D3D11_USAGE bufferUsage,
			UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0,
			UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0)
		{
			cameraBuffer.initBuffer(registerNumber, bufferUsage,
				CPUAccessFlags, miscFlags, structureByteStride,
				sysMemPitch, sysMemSlicePitch);

		}

		void setCameraBuffer()
		{
			if (!bufferUpdated)
			{
				cameraBuffer.setBufferData(std::vector<ViewProjectionMatrix>{ {(view * proj).Transpose()} });
				bufferUpdated = true;
			}
			cameraBuffer.setVertexShaderBuffer();
		}

		const DirectX::SimpleMath::Vector3& right()		const { return DirectX::SimpleMath::Vector3{ view._11, view._21, view._31 }; }
		const DirectX::SimpleMath::Vector3& up() 		const { return DirectX::SimpleMath::Vector3{ view._12, view._22, view._32 }; }
		const DirectX::SimpleMath::Vector3& forward() 	const { return DirectX::SimpleMath::Vector3{ view._13, view._23, view._33 }; }
		const DirectX::SimpleMath::Vector3& position()	const { return cameraPos; }


		/// <param name="fov">in radians</param>
		void setPerspective(float fov, float aspect, float zNear, float zFar)
		{
			proj = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, zNear, zFar);
			bufferUpdated = false;
		}

		void setView(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction, const DirectX::SimpleMath::Vector3& cameraUp)
		{
			cameraPos = position;

			view = DirectX::XMMatrixLookAtLH(position, position + direction, cameraUp);
			bufferUpdated = false;
		}

		DirectX::XMMATRIX getIPV()
		{
			if (!matricesUpdated)
			{
				updateMatrices();
			}
			

			return (proj * view).Invert();;
		}

		DirectX::XMMATRIX getViewMatrix()
		{
			if (!matricesUpdated)
			{
				updateMatrices();
			}
			return view;
		}

		DirectX::XMMATRIX getPerspectiveMatrix()
		{
			if (!matricesUpdated)
			{
				updateMatrices();
			}
			return proj;
		}

		void setWorldOffset(const DirectX::SimpleMath::Vector3& offset)
		{
			matricesUpdated = false;
			bufferUpdated = false;


			view._41 = offset.x;
			view._42 = offset.y;
			view._43 = offset.z;
			cameraPos = offset;

		}

		void addWorldOffset(const DirectX::SimpleMath::Vector3& offset)
		{
			matricesUpdated = false;
			bufferUpdated = false;


			view._41 = offset.x;
			view._42 = offset.y;
			view._43 = offset.z;


			cameraPos += offset;


		}

		void addRelativeOffset(const DirectX::SimpleMath::Vector3& offset) // relative to camera axis
		{
			updateBasis();

			matricesUpdated = false;
			bufferUpdated = false;

			//TODO:test it
			view._41 = view._41 * offset.x;
			view._42 = view._42 * offset.y;
			view._43 = view._43 * offset.z;


			cameraPos += offset;

		}

		void setWorldAngles(const Angles& angles)
		{
			basisUpdated = false;
			matricesUpdated = false;
			bufferUpdated = false;


			transform.rotation = transform.rotation, DirectX::XMQuaternionRotationAxis(DirectX::XMVECTOR{ 0.f, 0.f, 1.f, 0.f }, angles.roll);
			transform.rotation = DirectX::XMVectorMultiply(transform.rotation, DirectX::XMQuaternionRotationAxis(DirectX::XMVECTOR{ 1.f, 0.f, 0.f, 0.f }, angles.pitch));
			transform.rotation = DirectX::XMVectorMultiply(transform.rotation, DirectX::XMQuaternionRotationAxis(DirectX::XMVECTOR{ 0.f, 1.f, 0.f, 0.f }, angles.yaw));

			transform.rotation.Normalize();


		}

		void addWorldAngles(const Angles& angles)
		{
			basisUpdated = false;
			matricesUpdated = false;
			bufferUpdated = false;

			transform.rotation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0.f, 0.f, 1.f }, angles.roll);
			transform.rotation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 1.f, 0.f, 0.f }, angles.pitch);
			transform.rotation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0.f, 1.f, 0.f }, angles.yaw);

			transform.rotation.Normalize();

		}

		void addRelativeAngles(const Angles& angles)
		{
			basisUpdated = false;
			matricesUpdated = false;
			bufferUpdated = false;

			transform.rotation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0.f, 0.f, 1.f }, angles.roll);
			transform.rotation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 1.f, 0.f, 0.f }, angles.pitch);
			transform.rotation *= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0.f, 1.f, 0.f }, angles.yaw);

			transform.rotation.Normalize();

		}

		void updateBasis()
		{
			if (basisUpdated) return;
			basisUpdated = true;
			view = DirectX::SimpleMath::Matrix::CreateFromQuaternion(transform.rotation).Transpose() * view;


		}

		void updateMatrices()
		{
			if (matricesUpdated) return;
			matricesUpdated = true;
			updateBasis();

		}
	};



}

