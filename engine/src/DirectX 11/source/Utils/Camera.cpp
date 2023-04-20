#include "Camera.h"

namespace engine::DX
{
	void Camera::initBuffer(UINT registerNumber, UINT invCameraRegisterNumber, D3D11_USAGE bufferUsage, UINT CPUAccessFlags, UINT miscFlags, UINT structureByteStride, UINT sysMemPitch, UINT sysMemSlicePitch)
	{
		cameraBuffer.initBuffer(registerNumber, bufferUsage,
			CPUAccessFlags, miscFlags, structureByteStride,
			sysMemPitch, sysMemSlicePitch);

		invCameraBuffer.initBuffer(invCameraRegisterNumber, bufferUsage,
			CPUAccessFlags, miscFlags, structureByteStride,
			sysMemPitch, sysMemSlicePitch);

	}

	void Camera::setCameraBufferVertexShader()
	{
		updateCameraBuffer();
		cameraBuffer.setVertexShaderBuffer();
	}

	void Camera::setInvCameraBufferVertexShader()
	{
		updateMatrices();
		invCameraBuffer.setBufferData(std::vector<ViewProjectionMatrix>{ {getViewMatrix().Transpose().Invert(), getPerspectiveMatrix().Transpose().Invert()} });
		invCameraBuffer.setVertexShaderBuffer();
	}

	void Camera::setCameraBufferGeometryShader()
	{
		updateCameraBuffer();
		cameraBuffer.setGeometryShaderBuffer();
	}

	/// <param name="fov">in radians</param>
	void Camera::setPerspective(float fov, float aspect, float zNear, float zFar)
	{

		this->fov = fov;
		this->aspect = aspect;
		this->zNear = zNear;
		this->zFar = zFar;


		bufferUpdated = false;
	}

	void Camera::changeAspectRatio(float aspect)
	{
		this->aspect = aspect;
		bufferUpdated = false;
		projUpdated = false;
	}

	void Camera::setView(const float3& position, const float3& direction, const float3& worldUp)
	{
		this->worldUp = worldUp;
		this->target = position + direction;
		transform.position = position;
		viewUpdated = false;
		bufferUpdated = false;
	}

	float4x4 Camera::getIPV()
	{
		updateMatrices();
		return (getViewMatrix() * getPerspectiveMatrix()).Invert();;
	}

	const float4x4& Camera::getViewMatrix()
	{
		updateViewMatrix();
		return view;
	}

	const float4x4& Camera::getPerspectiveMatrix()
	{
		updatePerspectiveMatrix();
		return proj;
	}

	void Camera::setWorldOffset(const float3& offset)
	{
		matricesUpdated = false;
		viewUpdated = false;
		bufferUpdated = false;
		float3 direction = target - transform.position;
		transform.position = offset;
		target = offset + direction;
	}

	void Camera::addWorldOffset(const float3& offset)
	{
		matricesUpdated = false;
		viewUpdated = false;
		bufferUpdated = false;
		transform.position += offset;
		target += offset;
	}

	void Camera::addRelativeOffset(const float3& offset) // relative to camera axis
	{
		matricesUpdated = false;
		viewUpdated = false;
		bufferUpdated = false;
		transform.position += offset;
		target += offset;
	}

	void Camera::setWorldAngles(const Angles& angles)
	{
		matricesUpdated = false;
		viewUpdated = false;
		bufferUpdated = false;

		transform.rotation = quat::CreateFromAxisAngle({ 0.f, 1.f, 0.f }, angles.yaw) * transform.rotation;
		transform.rotation = transform.rotation * quat::CreateFromAxisAngle({ 1.f, 0.f, 0.f }, angles.pitch);
		transform.rotation.Normalize();


	}

	void Camera::addWorldAngles(const Angles& angles)
	{
		matricesUpdated = false;
		viewUpdated = false;
		bufferUpdated = false;

		transform.rotation = quat::CreateFromAxisAngle( { 0.f, 1.f, 0.f }, angles.yaw) * transform.rotation;
		transform.rotation = transform.rotation * quat::CreateFromAxisAngle( { 1.f, 0.f, 0.f }, angles.pitch);
		transform.rotation.Normalize();



	}

	void Camera::addRelativeAngles(const Angles& angles)
	{
		matricesUpdated = false;
		viewUpdated = false;
		bufferUpdated = false;

		
		transform.rotation = transform.rotation * quat::CreateFromAxisAngle({ 0.f, 1.f, 0.f }, angles.yaw);
		transform.rotation = transform.rotation * quat::CreateFromAxisAngle({ 1.f, 0.f, 0.f }, angles.pitch);
		transform.rotation.Normalize();

	}

	void Camera::updateCameraBuffer()
	{
		updateMatrices();
		if (!bufferUpdated)
		{
			cameraBuffer.setBufferData(std::vector<ViewProjectionMatrix>{ {getViewMatrix().Transpose(), getPerspectiveMatrix().Transpose()} });
			bufferUpdated = true;
		}
	}

	void Camera::updateViewMatrix()
	{
		if (viewUpdated) return;
		view = float4x4(DirectX::XMMatrixLookAtLH(transform.position, target, worldUp)) * float4x4::CreateFromQuaternion(transform.rotation);
		
		viewUpdated = true;
	}

	void Camera::updatePerspectiveMatrix()
	{
		if (projUpdated) return;
		proj = float4x4(DirectX::XMMatrixPerspectiveFovLH(fov, aspect, zNear, zFar)) * reverseDepthMatrix;
		projUpdated = true;
	}



	void Camera::updateMatrices()
	{
		if (matricesUpdated) return;
		updateViewMatrix();
		updatePerspectiveMatrix();
		matricesUpdated = true;
	}

}


