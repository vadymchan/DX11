#include "LightSystem.h"

namespace engine::DX
{



	LightSystem::PointLightID LightSystem::addPointLight(TransformSystem::ID transformID, const float3& color, float intensity, float radius,  const Attenuation& attenuation)
	{
		PointLight pointLight
		{
			{ color, intensity },
			transformID,
			radius,
			attenuation,

		};

		return pointLights.insert(pointLight);
	}

	LightSystem::DirectionalLightID LightSystem::addDirectionalLight(const float3& color, float intensity, const float3& direction, float solidAngle)
	{

		DirectionalLight light{ color, intensity, direction, solidAngle };

		return directionalLights.insert(light);
	}

	/// <param name="outerAngle"> pass cosine of degrees in radians (cos(radians(degrees)))</param>
	/// <param name="textureMask"> pass cosine of degrees in radians (cos(radians(degrees)))</param>
	LightSystem::SpotLightID LightSystem::addSpotLight(
		const float3& position,
		const float3& direction,
		const float3& color,
		float intensity,
		float innerAngle,
		float outerAngle,
		float radius,
		const Attenuation& attenuation)
	{
		float4x4 transform = float4x4::Identity;
		transform.Translation(position);
		transform._31 = direction.x;
		transform._32 = direction.y;
		transform._33 = direction.z;

		TransformSystem::ID transformID = TransformSystem::getInstance().addTransform(transform);

		SpotLight light
		{
			{ color, intensity },
			transformID,
			innerAngle,
			outerAngle,
			radius,
			attenuation
		};

		return spotLights.insert(light);
	}

	/// <param name="outerAngle"> pass cosine of degrees in radians (cos(radians(degrees)))</param>
	/// <param name="textureMask"> pass cosine of degrees in radians (cos(radians(degrees)))</param>
	LightSystem::FlashLightID LightSystem::addFlashLight(
		const float3& color,
		float intensity,
		float innerAngle,
		float outerAngle,
		float radius,
		const Attenuation& attenuation,
		const std::wstring& textureMask)
	{
		
		D3D11_TEXTURE2D_DESC desc{};
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		TextureManager::getInstance().addTexture2D(textureMask, FLASHLIGHT_TEXTURE_BIND_SLOT, desc);

		float3 position;
		float3 direction;
		bool attachedToCamera = false;
		if (camera != nullptr)
		{
			position = camera->position();
			direction = camera->forward();
			attachedToCamera = true;
		}

		const std::shared_ptr<Texture2D>& texture = TextureManager::getInstance().getTexture2D(textureMask);

		FlashLight flashLight =
		{
			{ color, intensity },
			innerAngle, outerAngle,
			radius,
			attenuation,
			attachedToCamera,
			position,
			direction,
			texture
		};

		return flashLights.insert(flashLight);
	}

	void LightSystem::updateTextureMasks()
	{
		for (size_t i = 0; i < flashLights.size(); ++i)
		{
			flashLights[i].textureMask->bind();
		}
	}

	void LightSystem::updateFlashLightConstantBuffer(FlashLight& src, LightConstantBuffer::FlashLight& dst)
	{
		if (src.attachedToCamera)
		{
			src.currentPosition = camera->position();
			src.currentDirection = camera->forward();
		}

		dst.position = src.currentPosition;
		dst.direction = src.currentDirection;

		//dst.position = float3::Transform(src.currentPosition, camera->getViewMatrix());
		//dst.direction = float3::TransformNormal(src.currentDirection, camera->getViewMatrix());
		dst.innerAngle = src.innerAngle;
		dst.outerAngle = src.outerAngle;
		dst.radius = src.radius;
		dst.attenuation.constantAttenuation = src.attenuation.constantAttenuation;
		dst.attenuation.linearAttenuation = src.attenuation.linearAttenuation;
		dst.attenuation.quadraticAttenuation = src.attenuation.quadraticAttenuation;
		dst.base.color = src.base.color;
		dst.base.intensity = src.base.intensity;
	}

	void LightSystem::updateSpotLightConstantBuffer(const SpotLight& src, LightConstantBuffer::SpotLight& dst)
	{

		dst.base.color = src.base.color;
		dst.base.intensity = src.base.intensity;
		dst.innerAngle = src.innerAngle;
		dst.outerAngle = src.outerAngle;
		dst.radius = src.radius;
		dst.attenuation.constantAttenuation = src.attenuation.constantAttenuation;
		dst.attenuation.linearAttenuation = src.attenuation.linearAttenuation;
		dst.attenuation.quadraticAttenuation = src.attenuation.quadraticAttenuation;
		const float4x4& worldMatrix = TransformSystem::getInstance().getTransform(src.transformID);
		dst.position = float3::Transform(worldMatrix.Translation(), camera->getViewMatrix());
		dst.direction = { worldMatrix._31, worldMatrix._32, worldMatrix._33 };
		//dst.position = float3::Transform(worldMatrix.Translation(), camera->getViewMatrix());
		dst.direction = float3::TransformNormal({ worldMatrix._31, worldMatrix._32, worldMatrix._33 }, camera->getViewMatrix());
	}

	void LightSystem::updateDirectionalLightConstantBuffer(const DirectionalLight& src, LightConstantBuffer::DirectionalLight& dst)
	{
		dst.base.color = src.base.color;
		dst.base.intensity = src.base.intensity;
		dst.solidAngle = src.solidAngle;
		//dst.direction = src.direction; // for world space
		dst.direction = float3::Transform(src.direction, camera->getViewMatrix());
	}

	void LightSystem::updatePointLightConstantBuffer(const PointLight& src, LightConstantBuffer::PointLight& dst)
	{
		dst.base.color = src.base.color;
		dst.base.intensity = src.base.intensity;
		dst.radius = src.radius;
		dst.attenuation.constantAttenuation = src.attenuation.constantAttenuation;
		dst.attenuation.linearAttenuation = src.attenuation.linearAttenuation;
		dst.attenuation.quadraticAttenuation = src.attenuation.quadraticAttenuation;
		const float4x4& worldMatrix = TransformSystem::getInstance().getTransform(src.transformID);
		//dst.position = worldMatrix.Transpose().Translation(); // for world space
		dst.position = float3::Transform(worldMatrix.Transpose().Translation(), camera->getViewMatrix());
	}

	void LightSystem::updateBuffer()
	{

		if (camera != nullptr)
		{
			m_buffer.createBuffer(); // creates buffer if it's not created yet

			LightConstantBuffer lightConstantBuffer;

			//add camera position
			//--------------------------------------------------------
			const float3& cameraPos = camera->position();
			lightConstantBuffer.g_cameraPosition = float4(cameraPos.x, cameraPos.y, cameraPos.z, 1);
			//--------------------------------------------------------

			//add flashlight
			//--------------------------------------------------------
			lightConstantBuffer.g_numFlashLights = flashLights.size();
			for (size_t i = 0; i < flashLights.size(); i++)
			{
				updateFlashLightConstantBuffer(flashLights[i], lightConstantBuffer.g_flashLights[i]);
			}
			//--------------------------------------------------------

			//directional lights
			//--------------------------------------------------------
			lightConstantBuffer.g_numDirectionalLights = directionalLights.size();
			for (size_t i = 0; i < directionalLights.size(); i++)
			{
				updateDirectionalLightConstantBuffer(directionalLights[i], lightConstantBuffer.g_directionalLights[i]);
			}
			//--------------------------------------------------------

			//pointLights
			//--------------------------------------------------------
			lightConstantBuffer.g_numPointLights = pointLights.size();
			for (size_t i = 0; i < pointLights.size(); i++)
			{
				updatePointLightConstantBuffer(pointLights[i], lightConstantBuffer.g_pointLights[i]);
			}
			//--------------------------------------------------------

			//spotlights
			//--------------------------------------------------------
			lightConstantBuffer.g_numSpotLights = spotLights.size();
			for (size_t i = 0; i < spotLights.size(); i++)
			{
				updateSpotLightConstantBuffer(spotLights[i], lightConstantBuffer.g_spotLights[i]);
			}
			//--------------------------------------------------------

			m_buffer.updateData({ lightConstantBuffer });

		}
	}

}