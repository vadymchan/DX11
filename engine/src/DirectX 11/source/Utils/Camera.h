#pragma once

#include "../D3D/D3D.h"
#include "../Buffer/ConstantBuffer.h"




namespace engine::DX
{
	

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
		float3 target;
		float3 worldUp;
		float4x4 proj;
		float4x4 view;
		float4x4 invView;
		float fov;
		float aspect;
		float zNear;
		float zFar;

		bool viewUpdated = false;
		bool projUpdated = false;	
		bool matricesUpdated = false;
		bool bufferUpdated = false;

		const float4x4 reverseDepthMatrix
		{
			{1,0,0,0},
			{0,1,0,0},
			{0,0,-1,0},
			{0,0,1,1},

		};

		struct ViewProjectionMatrix
		{
			float4x4 view;
			float4x4 proj;
		};

		ConstantBuffer<ViewProjectionMatrix> cameraBuffer;
		ConstantBuffer<ViewProjectionMatrix> invCameraBuffer;

		void updateCameraBuffer();

		void updateViewMatrix();

		void updatePerspectiveMatrix();

		void updateMatrices();

	public:

		

		void initBuffer(UINT registerNumber, D3D11_USAGE bufferUsage,
			UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0,
			UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0);

		void setCameraBufferVertexShader();
		void setInvCameraBufferVertexShader();
		void setCameraBufferGeometryShader();

		float3 right()		{ getViewMatrix(); return float3{ view._11, view._21, view._31 }; }
		float3 up() 		{ getViewMatrix(); return float3{ view._12, view._22, view._32 }; }
		float3 forward()	{ getViewMatrix(); return float3{ view._13, view._23, view._33 }; }

		const float3& position()	const { return transform.position; }
		float getFov() const { return fov; }
		float getAspectRatio() const { return aspect; }
		float getZNear() const { return zNear; }
		float getZFar() const { return zFar; }


		/// <param name="fov">in radians</param>
		void setPerspective(float fov, float aspect, float zNear, float zFar);

		void changeAspectRatio(float aspect);

		void setView(const float3& position, const float3& direction, const float3& cameraUp);

		float4x4 getIPV();

		const float4x4& getViewMatrix();

		const float4x4& getPerspectiveMatrix();

		void setWorldOffset(const float3& offset);

		void addWorldOffset(const float3& offset);

		void addRelativeOffset(const float3& offset);

		void setWorldAngles(const Angles& angles);

		void addWorldAngles(const Angles& angles);

		void addRelativeAngles(const Angles& angles);

		
	};



}

