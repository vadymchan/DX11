#pragma once

#include "../Math/Transform/TransformSystem.h"
#include "../Utils/SolidVector.h"
#include "../D3D/D3D.h"
#include "../Buffer/ConstantBuffer.h"

namespace engine::DX
{

	const UINT MAX_DIRECTIONAL_LIGHTS = 2;
	const UINT MAX_POINT_LIGHTS = 10;
	const UINT MAX_SPOT_LIGHTS = 10;
	const UINT MAX_FLASH_LIGHTS = 1;

	class LightSystem {
	public:

		

		struct Attenuation
		{
			float constantAttenuation;
			float linearAttenuation;
			float quadraticAttenuation;
		};

		struct BaseLight
		{
			float3 color;
			float intensity;
		};

		struct DirectionalLight
		{
			BaseLight base;
			float4 direction;
		};

		struct PointLight
		{
			BaseLight base;
			TransformSystem::ID transformID;
			Attenuation attenuation;
		};


		struct SpotLight
		{
			BaseLight base;
			TransformSystem::ID transformID;
			float innerAngle;
			float outerAngle;
			Attenuation attenuation;
		};

		struct FlashLight
		{
			BaseLight base;
			float innerAngle;
			float outerAngle;
			Attenuation attenuation;
			bool attachedToCamera;
			float3 currentPosition;
			float3 currentDirection;
			std::shared_ptr<Texture2D> textureMask;
		};

		using PointLightID = SolidVector<PointLight>::ID;
		using DirectionalLightID = SolidVector<DirectionalLight>::ID;
		using SpotLightID = SolidVector<SpotLight>::ID;
		using FlashLightID = SolidVector<FlashLight>::ID;

		struct LightConstantBuffer
		{

			/*struct BaseLight
			{
				float3 color;
				float intensity;
			};*/

			struct DirectionalLight
			{
				BaseLight base;
				float4 direction;
			};


			struct PointLight
			{
				BaseLight base;
				float3 position;
				float padding1;
				Attenuation attenuation;
				float padding2;

			};


			struct SpotLight
			{
				BaseLight base;
				float3 position;
				float innerAngle;
				float3 direction;
				float outerAngle;
				Attenuation attenuation;
				float padding;

			};

			struct FlashLight // currently the same as SpotLight (but may change in the future)
			{
				BaseLight base;
				float3 position;
				float innerAngle;
				float3 direction;
				float outerAngle;
				Attenuation attenuation;
				float padding;
			};

			int g_numDirectionalLights;
			int g_numPointLights;
			int g_numSpotLights;
			int g_numFlashLights;
			float4 g_cameraPosition;


			DirectionalLight g_directionalLights[MAX_DIRECTIONAL_LIGHTS];

			PointLight g_pointLights[MAX_POINT_LIGHTS];

			SpotLight g_spotLights[MAX_SPOT_LIGHTS];

			FlashLight g_flashLights[MAX_FLASH_LIGHTS];
		};

		

		static LightSystem& getInstance()
		{
			static LightSystem instance;
			return instance;
		}

		void setBuffer()
		{
			updateBuffer();
			updateTextureMasks();
			auto test = sizeof(LightConstantBuffer);
			m_buffer.setPixelShaderBuffer();
		}

		void setCamera(Camera& camera)
		{
			this->camera = &camera;
		}

		// Add a PointLight
		PointLightID addPointLight(
			TransformSystem::ID transformID,
			const float3& color,
			float intensity,
			const Attenuation& attenuation)
		{
			PointLight pointLight
			{
				{color, intensity},
				transformID,
				attenuation

			};
			
			return pointLights.insert(pointLight);
		}

		
		DirectionalLightID addDirectionalLight(const float3& color, float intensity, const float4& direction)
		{

			DirectionalLight light{ color, intensity, direction };

			return directionalLights.insert(light);
		}

		/// <param name="outerAngle"> pass cosine of degrees in radians (cos(radians(degrees)))</param>
		/// <param name="textureMask"> pass cosine of degrees in radians (cos(radians(degrees)))</param>
		SpotLightID addSpotLight(
			const float3& position,
			const float3& direction,
			const float3& color,
			float intensity,
			float innerAngle,
			float outerAngle,
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
				{color, intensity},
				transformID,
				innerAngle,
				outerAngle,
				attenuation
			};

			return spotLights.insert(light);
		}



		/// <param name="outerAngle"> pass cosine of degrees in radians (cos(radians(degrees)))</param>
		/// <param name="textureMask"> pass cosine of degrees in radians (cos(radians(degrees)))</param>
		FlashLightID addFlashLight(
			const float3& color,
			float intensity,
			float innerAngle,
			float outerAngle,
			const Attenuation& attenuation,
			const std::wstring& textureMask)
		{
			const UINT TEXTURE_MASK = 1;
			D3D11_TEXTURE2D_DESC desc{};
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			TextureManager::getInstance().addTexture2D(textureMask, TEXTURE_MASK, desc);

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
				{color, intensity},
				innerAngle, outerAngle,
				attenuation,
				attachedToCamera,
				position,
				direction,
				texture
			};

			return flashLights.insert(flashLight);
		}

		// Remove methods
		void removePointLight(PointLightID id)
		{
			pointLights.erase(id);

		}

		void removeDirectionalLight(DirectionalLightID id)
		{
			directionalLights.erase(id);

		}

		FlashLight& getFlashLight(FlashLightID id)
		{
			return flashLights[id];
		}

		void removeSpotLight(SpotLightID id)
		{
			spotLights.erase(id);

		}

		// Remove a FlashLight
		void removeFlashLight(FlashLightID id)
		{
			flashLights.erase(id);

		}

		// Update methods
		void updatePointLight(PointLightID id, const PointLight& newLight)
		{
			pointLights[id] = newLight;

		}

		void updateDirectionalLight(DirectionalLightID id, const DirectionalLight& newLight)
		{
			directionalLights[id] = newLight;

		}

		void updateSpotLight(SpotLightID id, const SpotLight& newLight)
		{
			spotLights[id] = newLight;

		}

		

	protected:
		LightSystem()
		{
			const UINT PER_FRAME_GLOBAL = 4;
			m_buffer.setBufferData({ LightConstantBuffer{} });
			m_buffer.initBuffer(PER_FRAME_GLOBAL, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

		}

		LightSystem(const LightSystem&) = delete;
		LightSystem& operator=(const LightSystem&) = delete;

		void updateTextureMasks()
		{
			for (size_t i = 0; i < flashLights.size(); ++i)
			{
				flashLights[i].textureMask->bind();
			}
		}

		void updateFlashLightConstantBuffer(FlashLight& src, LightConstantBuffer::FlashLight& dst)
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
			dst.attenuation.constantAttenuation = src.attenuation.constantAttenuation;
			dst.attenuation.linearAttenuation = src.attenuation.linearAttenuation;
			dst.attenuation.quadraticAttenuation = src.attenuation.quadraticAttenuation;
			dst.base.color = src.base.color;
			dst.base.intensity = src.base.intensity;
		}

		void updateSpotLightConstantBuffer(const SpotLight& src, LightConstantBuffer::SpotLight& dst)
		{
			

			dst.base.color = src.base.color;
			dst.base.intensity = src.base.intensity;
			dst.innerAngle = src.innerAngle;
			dst.outerAngle = src.outerAngle;
			dst.attenuation.constantAttenuation = src.attenuation.constantAttenuation;
			dst.attenuation.linearAttenuation = src.attenuation.linearAttenuation;
			dst.attenuation.quadraticAttenuation = src.attenuation.quadraticAttenuation;
			const float4x4& worldMatrix = TransformSystem::getInstance().getTransform(src.transformID);
			dst.position = float3::Transform(worldMatrix.Translation(), camera->getViewMatrix());
			dst.direction = { worldMatrix._31, worldMatrix._32, worldMatrix._33 };
			//dst.position = float3::Transform(worldMatrix.Translation(), camera->getViewMatrix());
			dst.direction = float3::TransformNormal({ worldMatrix._31, worldMatrix._32, worldMatrix._33 }, camera->getViewMatrix()) ;
		}

		void updateDirectionalLightConstantBuffer(const DirectionalLight& src, LightConstantBuffer::DirectionalLight& dst)
		{

			dst.base.color = src.base.color;
			dst.base.intensity = src.base.intensity;
			dst.direction = float4::Transform(src.direction, camera->getViewMatrix());
			//dst.direction = src.direction;
		}

		void updatePointLightConstantBuffer(const PointLight& src, LightConstantBuffer::PointLight& dst)
		{
			dst.base.color = src.base.color;
			dst.base.intensity = src.base.intensity;
			dst.attenuation.constantAttenuation = src.attenuation.constantAttenuation;
			dst.attenuation.linearAttenuation = src.attenuation.linearAttenuation;
			dst.attenuation.quadraticAttenuation = src.attenuation.quadraticAttenuation;
			const float4x4& worldMatrix = TransformSystem::getInstance().getTransform(src.transformID);
			dst.position = float3::Transform(worldMatrix.Transpose().Translation(), camera->getViewMatrix());
		}

		void updateBuffer()
		{

			if (camera != nullptr)
			{
				m_buffer.createBuffer(); // creates buffer if it's not created yet

				LightConstantBuffer lightConstantBuffer;

				const float3& cameraPos = camera->position();
				lightConstantBuffer.g_cameraPosition = float4(cameraPos.x, cameraPos.y, cameraPos.z, 1);
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

		// Store lights in separate containers for each type
		SolidVector<PointLight> pointLights;
		SolidVector<DirectionalLight> directionalLights;
		SolidVector<SpotLight> spotLights;
		SolidVector<FlashLight> flashLights;

		Camera* camera{};

		ConstantBuffer<LightConstantBuffer> m_buffer;
		



	};

}

