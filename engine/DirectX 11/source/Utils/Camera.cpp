#include "Camera.h"

namespace engine::DX
{
	void Camera::initBuffer(UINT registerNumber, D3D11_USAGE bufferUsage, UINT CPUAccessFlags, UINT miscFlags, UINT structureByteStride, UINT sysMemPitch, UINT sysMemSlicePitch)
	{
		cameraBuffer.initBuffer(registerNumber, bufferUsage,
			CPUAccessFlags, miscFlags, structureByteStride,
			sysMemPitch, sysMemSlicePitch);

	}

	void Camera::setCameraBuffer()
	{
		updateMatrices();
		if (!bufferUpdated)
		{
			cameraBuffer.setBufferData(std::vector<ViewProjectionMatrix>{ {(view * proj).Transpose()} });
			bufferUpdated = true;
		}
		cameraBuffer.setVertexShaderBuffer();
	}

	/// <param name="fov">in radians</param>

	void Camera::setPerspective(float fov, float aspect, float zNear, float zFar)
	{

		this->fov = fov;
		this->aspect = aspect;
		this->zNear = zNear;
		this->zFar = zFar;

		proj = float4x4(DirectX::XMMatrixPerspectiveFovLH(fov, aspect, zNear, zFar)) * reverseDepthMatrix;

		bufferUpdated = false;
	}

	void Camera::changeAspectRatio(float aspect)
	{
		proj = float4x4(DirectX::XMMatrixPerspectiveFovLH(fov, aspect, zNear, zFar)) * reverseDepthMatrix;
		bufferUpdated = false;
	}

	void Camera::setView(const float3& position, const float3& direction, const float3& cameraUp)
	{
		cameraPos = position;
		view = DirectX::XMMatrixLookToLH(position, direction, cameraUp);
		bufferUpdated = false;
	}

	const float4x4& Camera::getIPV()
	{
		if (!matricesUpdated)
		{
			updateMatrices();
		}


		return (view * proj).Invert();;
	}

	const float4x4& Camera::getViewMatrix()
	{
		if (!matricesUpdated)
		{
			updateMatrices();
		}
		return view;
	}

	const float4x4& Camera::getPerspectiveMatrix()
	{
		if (!matricesUpdated)
		{
			updateMatrices();
		}
		return proj;
	}

	void Camera::setWorldOffset(const float3& offset)
	{
		matricesUpdated = false;
		bufferUpdated = false;


		view._41 = -offset.x;
		view._42 = -offset.y;
		view._43 = -offset.z;
		cameraPos = offset;

	}

	void Camera::addWorldOffset(const float3& offset)
	{
		matricesUpdated = false;
		bufferUpdated = false;
		updateBasis();


		view._41 -= offset.x;
		view._42 -= offset.y;
		view._43 -= offset.z;


		cameraPos += offset;


	}

	void Camera::addRelativeOffset(const float3& offset) // relative to camera axis
	{
		matricesUpdated = false;
		bufferUpdated = false;

		updateBasis();

		view._41 -= offset.x;
		view._42 -= offset.y;
		view._43 -= offset.z;


		cameraPos += offset;

	}

	void Camera::setWorldAngles(const Angles& angles)
	{
		basisUpdated = false;
		matricesUpdated = false;
		bufferUpdated = false;

		transform.rotation = quat::CreateFromAxisAngle({ 0.f, 0.f, 1.f }, angles.roll);
		transform.rotation = quat::CreateFromAxisAngle({ 1.f, 0.f, 0.f }, angles.pitch) * transform.rotation;
		transform.rotation = quat::CreateFromAxisAngle({ 0.f, 1.f, 0.f }, angles.yaw) * transform.rotation;

		transform.rotation.Normalize();


		transform.position = { angles.pitch, angles.yaw, angles.roll };

	}

	void Camera::addWorldAngles(const Angles& angles)
	{
		basisUpdated = false;
		matricesUpdated = false;
		bufferUpdated = false;

		transform.rotation *= quat::CreateFromAxisAngle({ 0.f, 0.f, 1.f }, angles.roll);
		transform.rotation *= quat::CreateFromAxisAngle({ 1.f, 0.f, 0.f }, angles.pitch);
		transform.rotation *= quat::CreateFromAxisAngle({ 0.f, 1.f, 0.f }, angles.yaw);

		transform.rotation.Normalize();

	}

	void Camera::addRelativeAngles(const Angles& angles)
	{
		basisUpdated = false;
		matricesUpdated = false;
		bufferUpdated = false;

		transform.rotation *= quat::CreateFromAxisAngle(forward(), angles.roll);
		transform.rotation *= quat::CreateFromAxisAngle(right(), angles.pitch);
		transform.rotation *= quat::CreateFromAxisAngle(up(), angles.yaw);


		transform.rotation.Normalize();

	}

	void Camera::updateBasis()
	{
		if (basisUpdated) return;
		basisUpdated = true;

		view *= float4x4::CreateFromQuaternion(transform.rotation);

	}

	void Camera::updateMatrices()
	{
		if (matricesUpdated) return;
		matricesUpdated = true;
		updateBasis();

	}

}


