#pragma once

#include "../Math/Transform/TransformSystem.h"
#include "../Utils/SolidVector.h"
#include "../D3D/D3D.h"
#include "../Buffer/ConstantBuffer.h"
#include "../Controller/BufferManager.h"
#include "../Texture/Texture2D.h"
#include "../Utils/Camera.h"
#include "../Controller/TextureManager.h"

namespace engine::DX
{

	const UINT MAX_DIRECTIONAL_LIGHTS = 2;
	const UINT MAX_POINT_LIGHTS = 4;
	const UINT MAX_SPOT_LIGHTS = 3;
	const UINT MAX_FLASH_LIGHTS = 1;

	const UINT SHADOW_MAP_SIZE = 1024;

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
			float3 direction;
			float solidAngle;
		};

		struct PointLight
		{
			BaseLight base;
			TransformSystem::ID transformID;
			float radius;
			Attenuation attenuation;
		};


		struct SpotLight
		{
			BaseLight base;
			TransformSystem::ID transformID;
			float innerAngle;
			float outerAngle;
			float radius;
			float3 direction;
			Attenuation attenuation;
		};

		struct FlashLight
		{
			BaseLight base;
			float innerAngle;
			float outerAngle;
			float radius;
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

		int getCurrentPointLights() const
		{
			return pointLights.size();
		}

		int getCurrentFlashLights() const
		{
			return flashLights.size();
		}

		int getCurrentSpotLights() const
		{
			return spotLights.size();
		}

		int getCurrentDirectionalLights() const
		{
			return directionalLights.size();
		}

		// binding PerView constant buffer for shadow map generation
		void setPerViewDirectionalLight(int lightIndex);

		// binding PerView constant buffer for shadow map generation
		void setPerViewSpotLight(int lightIndex);

		// binding PerView constant buffer for shadow map generation
		void setPerViewFlashLight(int lightIndex);

		// binding PerView constant buffer for shadow map generation
		void setPerViewPointLight(int lightIndex);

		void bindDirectionalLightShadowMapTextures()
		{
			directionalLightShadowMap.bind();
		}

		void bindSpotLightShadowMapTextures()
		{
			spotLightShadowMap.bind();
		}

		void bindPointLightShadowMapTextures()
		{
			pointLightShadowMap.bind();
		}

		void bindFlashLightShadowMapTextures()
		{
			flashLightShadowMap.bind();
		}

		void bindShadowMapTextures()
		{
			bindDirectionalLightShadowMapTextures();
			bindSpotLightShadowMapTextures();
			bindFlashLightShadowMapTextures();
			bindPointLightShadowMapTextures();

		}

		void clearShadwoMaps()
		{
			for (size_t i = 0; i < directionalLights.size(); ++i)
			{
				g_devcon->ClearDepthStencilView(directionalLightViews[i].Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);
			}
			for (size_t i = 0; i < spotLightViews.size(); ++i)
			{
				g_devcon->ClearDepthStencilView(spotLightViews[i].Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);
			}
			for (size_t i = 0; i < flashLightViews.size(); ++i)
			{
				g_devcon->ClearDepthStencilView(flashLightViews[i].Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);
			}
			for (size_t i = 0; i < pointLightViews.size(); ++i)
			{
				g_devcon->ClearDepthStencilView(pointLightViews[i].Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);
			}
		}


		PointLightID addPointLight(const PointLight& pointLight);

		/// <param name="solidAngle">in steradians</param>
		DirectionalLightID addDirectionalLight(const DirectionalLight& directionalLight);

		SpotLightID addSpotLight(const SpotLight& spotLight);

		/// <param name="outerAngle"> pass cosine of degrees in radians (cos(radians(degrees)))</param>
		/// <param name="textureMask"> pass cosine of degrees in radians (cos(radians(degrees)))</param>
		FlashLightID addFlashLight(
			const float3& color,
			float intensity,
			float innerAngle,
			float outerAngle,
			float radius,
			const Attenuation& attenuation,
			const std::wstring& textureMask);

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

		void removeFlashLight(FlashLightID id)
		{
			flashLights.erase(id);
		}

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

		struct PerView2dShadowMap
		{
			float4x4 viewMatrix;
			float4x4 projMatrix;
		};

		struct PerView3dShadowMap
		{
			std::array<float4x4, 6> viewMatrix;
			float4x4 projMatrix;
		};

		void bind2DShadowMap(const PerView2dShadowMap& constantBuffer)
		{
			perView2dConstantBuffer.updateData({ constantBuffer });
			perView2dConstantBuffer.setVertexShaderBuffer();
		}

		ConstantBuffer<PerView2dShadowMap> perView2dConstantBuffer;
		ConstantBuffer<PerView3dShadowMap> perView3dConstantBuffer;

		struct LightConstantBuffer
		{

			struct DirectionalLight
			{
				BaseLight base;
				float3 direction;
				float solidAngle;
				float4x4 shadowViewMatrix;
				float4x4 shadowProjMatrix;
			};


			struct PointLight
			{
				BaseLight base;
				float3 position;
				float radius;
				Attenuation attenuation;
				float padding;
				std::array<float4x4, 6> shadowViewMatrices; // 6 - number of faces of the cube map
				float4x4 shadowProjMatrix;

			};


			struct SpotLight
			{
				BaseLight base;
				float3 position;
				float innerAngle;
				float3 direction;
				float outerAngle;
				float radius;
				Attenuation attenuation;
				float4x4 shadowViewMatrix;
				float4x4 shadowProjMatrix;

			};


			struct FlashLight // currently the same as SpotLight (but may change in the future)
			{
				BaseLight base;
				float3 position;
				float innerAngle;
				float3 direction;
				float outerAngle;
				float radius;
				Attenuation attenuation;
				float4x4 shadowViewMatrix;
				float4x4 shadowProjMatrix;
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

		LightSystem()
		{

			m_buffer.setBufferData({ LightConstantBuffer{} });
			m_buffer.initBuffer(PER_FRAME_GLOBAL, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

			perView2dConstantBuffer.setBufferData({ PerView2dShadowMap{} });
			perView2dConstantBuffer.initBuffer(PER_VIEW_SHADOW_MAP_2D, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
			perView2dConstantBuffer.createBuffer();

			perView3dConstantBuffer.setBufferData({ PerView3dShadowMap{} });
			perView3dConstantBuffer.initBuffer(PER_VIEW_SHADOW_MAP_3D, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
			perView3dConstantBuffer.createBuffer();

			initTexture2DShadowMap(MAX_DIRECTIONAL_LIGHTS, DIRECTIONAL_LIGHT_SHADOW_MAPS_SLOT, directionalLightShadowMap, directionalLightViews);
			initTexture2DShadowMap(MAX_SPOT_LIGHTS, SPOT_LIGHT_SHADOW_MAPS_SLOT, spotLightShadowMap, spotLightViews);
			initTexture2DShadowMap(MAX_FLASH_LIGHTS, FLASH_LIGHT_SHADOW_MAPS_SLOT, flashLightShadowMap, flashLightViews);
			initCubeShadowMap(MAX_POINT_LIGHTS, POINT_LIGHT_SHADOW_MAPS_SLOT, pointLightShadowMap, pointLightViews);
		}

		LightSystem(const LightSystem&) = delete;
		LightSystem& operator=(const LightSystem&) = delete;

		void updateTextureMasks();

		void updateFlashLightConstantBuffer(FlashLight& src, LightConstantBuffer::FlashLight& dst);

		void updateSpotLightConstantBuffer(const SpotLight& src, LightConstantBuffer::SpotLight& dst);

		void updateDirectionalLightConstantBuffer(const DirectionalLight& src, LightConstantBuffer::DirectionalLight& dst);

		void updatePointLightConstantBuffer(const PointLight& src, LightConstantBuffer::PointLight& dst);

		void updateBuffer();

		void initTexture2DShadowMap(UINT arraySize, UINT bindSlot, Texture2D& shadowMaps, std::vector<ComPtr<ID3D11DepthStencilView>>& depthStencilViews);

		void initCubeShadowMap(UINT arraySize, UINT bindSlot, Texture2D& shadowMaps, std::vector<ComPtr<ID3D11DepthStencilView>>& depthStencilViews);

		void computeLightSpaceFrustumDirectionalLight(const float3& lightDir, float4x4& lightViewMatrix, float4x4& lightProjMatrix);

		// general function for computing the view and perspective matrices for both spot and flash lights
		void computeBeamLightMatrices(
			float fov,
			const float3& lightPos,
			const float3& lightDir,
			float nearPlane,
			float farPlane,
			float4x4& lightViewMatrix,
			float4x4& lightProjMatrix);

		void computePointLightMatrices(const float3& lightPos, float nearPlane, float farPlane, std::array<float4x4, 6>& lightViewMatrices, float4x4& lightProjMatrix);



		Camera* camera{};

		ConstantBuffer<LightConstantBuffer> m_buffer;
		LightConstantBuffer m_lightConstantBuffer;

		// Store lights in separate containers for each type
		SolidVector<PointLight> pointLights;
		SolidVector<DirectionalLight> directionalLights;
		SolidVector<SpotLight> spotLights;
		SolidVector<FlashLight> flashLights;

		//store the shadow map of the light
		Texture2D directionalLightShadowMap;
		Texture2D spotLightShadowMap;
		Texture2D pointLightShadowMap;
		Texture2D flashLightShadowMap;
		std::vector<ComPtr<ID3D11DepthStencilView>>	directionalLightViews;
		std::vector<ComPtr<ID3D11DepthStencilView>>	spotLightViews;
		std::vector<ComPtr<ID3D11DepthStencilView>>	pointLightViews;
		std::vector<ComPtr<ID3D11DepthStencilView>>	flashLightViews;

	};

}

