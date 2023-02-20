#pragma once

#include "D3D/D3D.h"





namespace engine::DX
{
	//using variables as in HLSL
	using float2 = DirectX::SimpleMath::Vector2;
	using float3 = DirectX::SimpleMath::Vector3;
	using float4 = DirectX::SimpleMath::Vector4;
	using float4x4 = DirectX::SimpleMath::Matrix;

	using quat = DirectX::SimpleMath::Quaternion;

	struct Transform
	{
		quat rotation;
		float3 position;
		float3 scale;
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

		float4x4 proj;
		float4x4 view;
		float3 cameraPos;
		float fov;
		float aspect;
		float zNear;
		float zFar;

		bool basisUpdated = false;
		bool matricesUpdated = false;
		bool bufferUpdated = false;

		const float4x4 reverseDepthMatrix
		{
			{1,0,0,0},
			{0,1,0,0},
			{0,0,-1,1},
			{0,0,0,1},

		};

		

	public:

		struct ViewProjectionMatrix
		{
			//float4x4 view;
			//float4x4 proj;
			float4x4 viewProj;
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
			updateMatrices();
			if (!bufferUpdated)
			{
				cameraBuffer.setBufferData(std::vector<ViewProjectionMatrix>{ {(view * proj).Transpose()} });
				bufferUpdated = true;
			}
			cameraBuffer.setVertexShaderBuffer();
		}

		//const float3& right()		const { return float3{ view._11, view._21, view._31 }; }
		//const float3& up() 			const { return float3{ view._12, view._22, view._32 }; }
		//const float3& forward() 	const { return float3{ view._13, view._23, view._33 }; }

		//const float3& right()		const { return float3{ view._11, view._12, view._13 }; }
		//const float3& up() 		const { return float3{ view._21, view._22, view._23 }; }
		//const float3& forward() 	const { return float3{ view._31, view._32, view._33 }; }

		const float3& right()		const { return view.Right(); }
		const float3& up() 		const { return view.Up(); }
		const float3& forward() 	const { return -view.Forward(); } //because SimpleMath uses right-handed coordinate system
		const float3& position()	const { return cameraPos; }
		float getFov() const { return fov; }
		float getAspectRatio() const { return aspect; }
		float getZNear() const { return zNear; }
		float getZFar() const { return zFar; }


		/// <param name="fov">in radians</param>
		void setPerspective(float fov, float aspect, float zNear, float zFar)
		{

			this->fov = fov;
			this->aspect = aspect;
			this->zNear = zNear;
			this->zFar = zFar;
			//proj = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, zNear, zFar);
			//proj = reverseDepthMatrix * DirectX::XMMatrixPerspectiveFovLH(fov, aspect, zNear, zFar);
			proj = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, zNear, zFar);
			//proj = proj * reverseDepthMatrix;
			bufferUpdated = false;
		}

		void changeAspectRatio(float aspect)
		{
			//proj = reverseDepthMatrix * DirectX::XMMatrixPerspectiveFovLH(fov, aspect, zNear, zFar);
			proj = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, zNear, zFar);
			//proj = proj * reverseDepthMatrix;

			bufferUpdated = false;
		}

		void setView(const float3& position, const float3& direction, const float3& cameraUp)
		{
			cameraPos = position;

			//auto dir{ position - direction };
			//auto t = DirectX::XMMatrixLookToLH(position, dir, cameraUp);
			view = DirectX::XMMatrixLookToLH(position, direction, cameraUp);
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

		void setWorldOffset(const float3& offset)
		{
			matricesUpdated = false;
			bufferUpdated = false;


			view._41 = -offset.x;
			view._42 = -offset.y;
			view._43 = -offset.z;
			cameraPos = offset;

		}

		void addWorldOffset(const float3& offset)
		{
			matricesUpdated = false;
			bufferUpdated = false;
			updateBasis();


			view._41 -= offset.x;
			view._42 -= offset.y;
			view._43 -= offset.z;


			cameraPos += offset;


		}

		void addRelativeOffset(const float3& offset) // relative to camera axis
		{


			matricesUpdated = false;
			bufferUpdated = false;

			updateBasis();


			//float3 relativeOffset{ offset.x * right() + offset.y * up() + offset.z * forward() };

			//std::cout << relativeOffset.x << '\t' << relativeOffset.y << '\t' << relativeOffset.z << std::endl;


			//view.Translation(view.Translation() - relativeOffset);


			/*view._41 -= view._41 * relativeOffset.x;
			view._42 -= view._42 * relativeOffset.y;
			view._43 -= view._43 * relativeOffset.z;*/

			view._41 -= offset.x;
			view._42 -= offset.y;
			view._43 -= offset.z;


			cameraPos += offset;

		}

		void setWorldAngles(const Angles& angles)
		{
			basisUpdated = false;
			matricesUpdated = false;
			bufferUpdated = false;

			transform.rotation = quat::CreateFromAxisAngle({ 0.f, 0.f, 1.f }, angles.roll);
			transform.rotation = quat::CreateFromAxisAngle({ 1.f, 0.f, 0.f }, angles.pitch) * transform.rotation;
			transform.rotation = quat::CreateFromAxisAngle({ 0.f, 1.f, 0.f }, angles.yaw) * transform.rotation;

			//transform.rotation = quat::CreateFromAxisAngle(forward(), angles.roll);
			//transform.rotation *= quat::CreateFromAxisAngle(right(), angles.pitch);
			//transform.rotation *= quat::CreateFromAxisAngle(up(), angles.yaw);

			transform.rotation.Normalize();


			transform.position = { angles.pitch, angles.yaw, angles.roll };

		}

		void addWorldAngles(const Angles& angles)
		{
			basisUpdated = false;
			matricesUpdated = false;
			bufferUpdated = false;

			transform.rotation *= quat::CreateFromAxisAngle({ 0.f, 0.f, 1.f }, angles.roll);
			transform.rotation *= quat::CreateFromAxisAngle({ 1.f, 0.f, 0.f }, angles.pitch);
			transform.rotation *= quat::CreateFromAxisAngle({ 0.f, 1.f, 0.f }, angles.yaw);

			transform.rotation.Normalize();

		}

		void addRelativeAngles(const Angles& angles)
		{
			basisUpdated = false;
			matricesUpdated = false;
			bufferUpdated = false;

			transform.rotation *= quat::CreateFromAxisAngle(forward(), angles.roll);
			transform.rotation *= quat::CreateFromAxisAngle(right(), angles.pitch);
			transform.rotation *= quat::CreateFromAxisAngle(up(), angles.yaw);


			transform.rotation.Normalize();

		}

		void updateBasis()
		{
			if (basisUpdated) return;
			basisUpdated = true;

			//view = float4x4::CreateFromYawPitchRoll(transform.position).Transpose() * view;
			//view *= float4x4::CreateFromYawPitchRoll(transform.position);

			view *= float4x4::CreateFromQuaternion(transform.rotation);

			std::cout << "right\t" << right().x << '\t' << right().y << '\t' << right().z << std::endl;
			std::cout << "up\t" << up().x << '\t' << up().y << '\t' << up().z << std::endl;
			std::cout << "forward\t" << forward().x << '\t' << forward().y << '\t' << forward().z << std::endl << std::endl;
		}

		void updateMatrices()
		{
			if (matricesUpdated) return;
			matricesUpdated = true;
			updateBasis();

		}
	};



}

