#pragma once
#include "OpaqueInstances.h"
#include "../Math/Colision/ModelTriangleOctreeDX.h"
#include "../Controller/ModelManager.h"
#include "../../../general/utils/timer/FPSTimerRC.h"	 
#include "../Utils/Camera.h"
#include "../Light/LightSystem.h"
#include "../Texture/ReflectionCapture.h"
#include "../Texture/Skybox.h"
#include "../Controller/ImGuiManager.h"

namespace engine::DX
{
	using Intersection = MeshTriangleOctree::Intersection;
	class MeshSystem
	{
	public:

		MeshSystem(const MeshSystem&) = delete;
		MeshSystem& operator=(const MeshSystem&) = delete;
		static MeshSystem& getInstance()
		{
			static MeshSystem instance;
			return instance;
		}

		void render(Camera& camera);

		bool findIntersection(const ray& r, Instance& instance, Intersection& intersection);

		/// <param name="instance">instance, which is contained in opaqueInstance</param>
		void updateOpaqueInstanceBuffer(const Instance instance);

		//skybox is needed for taking the right environment map
		void setSkybox(std::shared_ptr<Skybox> skybox)
		{
			currentSkybox = skybox;
		}

		void setPerFrameIBLbuffer(std::shared_ptr<ConstantBuffer<PerFrameIBL>> perFrameIBLbuffer)
		{
			m_perFrameIBLbuffer = perFrameIBLbuffer;
		}

		void setShowNormal(bool showNormal)
		{
			m_showNormal = showNormal;
		}

		/// <param name="instances">instance may be change during dragging</param>
		void addInstances(uint32_t opaqueInstanceID,
			const std::shared_ptr<Model>& model,
			const std::vector<size_t>& meshIndex,
			const std::shared_ptr<OpaqueInstances::Material>& material,
			const std::vector<Instance>& transformID);

		/// <summary>
		/// creates new Opaque Instance
		/// </summary>
		/// <param name = "shaderFileNames">0 - vertex, 1 - hull shader, 2 - domain shader, 3 - geometry shader, 4 - pixel shader file names</param>
		/// <returns>ID to access to created opaque instance</returns>
		uint32_t createOpaqueInstance(const std::vector<std::array<std::wstring, (int)OpaqueInstances::ShaderType::SHADER_TYPE_NUM>>& shaderFileNames);


		ConstantBuffer<DirectX::SimpleMath::Vector4> cameraPosition;
		ConstantBuffer<DirectX::SimpleMath::Vector4> time;

	private:
		MeshSystem()
		{
			cameraPosition.initBuffer(PER_VIEW_PIXEL_HOLOGRAM_SHADER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
			time.initBuffer(PER_DRAW_HOLOGRAM_SHADER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

			m_perFrameIBL.useIBL = true;
			m_perFrameIBL.useDiffuseReflection = true;
			m_perFrameIBL.useSpecularReflection = true;
			m_perFrameIBL.useRoughnessOverwriting = false;
			m_perFrameIBL.overwrittenRoughnessValue = 0.0f;

			ImGuiManager::getInstance().AddValueChangedCallback([&](const std::string& identifier, float value) {
				// Cast the value to bool for checkbox
				bool boolValue = value > 0.0f;

				if (identifier == "useDiffuseReflection")
				{
					m_perFrameIBL.useDiffuseReflection = boolValue;
				}
				else if (identifier == "useSpecularReflection")
				{
					m_perFrameIBL.useSpecularReflection = boolValue;
				}
				else if (identifier == "useIBL")
				{
					m_perFrameIBL.useIBL = boolValue;
				}
				else if (identifier == "useRoughnessOverwriting")
				{
					m_perFrameIBL.useRoughnessOverwriting = boolValue;
				}
				else if (identifier == "overwrittenRoughnessValue")
				{
					m_perFrameIBL.overwrittenRoughnessValue = value;
				}
			});



		};
		std::vector<std::shared_ptr<OpaqueInstances>> opaqueInstances{};
		std::vector<std::shared_ptr<ModelTriangleOctree>> triangleOctrees{};
		std::weak_ptr<Skybox> currentSkybox; // used to take skybox for taking the right environment map

		bool m_showNormal{};
		std::weak_ptr<ConstantBuffer<PerFrameIBL>> m_perFrameIBLbuffer;
		PerFrameIBL m_perFrameIBL;

		struct ModelIntersection
		{
			std::weak_ptr<ModelTriangleOctree> triangleOctree;
			Instance instance;
			std::weak_ptr<OpaqueInstances> opaqueInstance; // used to update instance buffer during dragging
		};

		std::vector<ModelIntersection> modelIntersections{};

		enum class RenderMode
		{
			DEFAULT,
			NORMAL_VISUALISER,
			HOLOGRAM,
			BLINN_PHONG,
			POINT_SPHERE,
			PBR,
			IBL,
		};

		template<RenderMode Mode>
		void SetRenderMode(OpaqueInstances* opaqueInstance = nullptr, Camera* camera = nullptr) {};

		template<>
		void SetRenderMode<RenderMode::HOLOGRAM>(OpaqueInstances* opaqueInstance, Camera* camera)
		{

			time.setBufferData(std::vector<DirectX::SimpleMath::Vector4>{ {general::FPSTimer::getCurrentTick() - general::FPSTimer::initTick, 0, 0, 0}});
			time.setBuffer();

			float3 cameraPos = camera->position();
			cameraPosition.setBufferData(std::vector<DirectX::SimpleMath::Vector4>{ {cameraPos.x, cameraPos.y, cameraPos.z, 1.0}});
			cameraPosition.setPixelShaderBuffer();
			camera->setCameraBufferGeometryShader();
		}

		//set textures to pixel shader (irradiance, prefilteredEnvMap, brdfLUT)
		template<>
		void SetRenderMode<RenderMode::IBL>(OpaqueInstances* opaqueInstance, Camera* camera)
		{
			std::wstring environmentMap = std::filesystem::path(currentSkybox.lock()->getTexture()).stem().wstring();

			const std::wstring irradianceMapFileName = environmentMap + L"_Irradiance.dds";
			const std::wstring irradianceMapPath = ReflectionCapture::PRECOMPUTE_IRRADIANCE_PATH / irradianceMapFileName;
			std::shared_ptr<Texture2D> irradianceMap = TextureManager::getInstance().getTexture2D(irradianceMapPath);
			const std::wstring prefilteredMapFileName = environmentMap + L"_Prefiltered.dds";
			const std::wstring prefilteredMapPath = ReflectionCapture::PRECOMPUTE_PREFILTERED_PATH / prefilteredMapFileName;
			std::shared_ptr<Texture2D> prefilteredMap = TextureManager::getInstance().getTexture2D(prefilteredMapPath);
			const std::wstring lutTextureFileName = ReflectionCapture::LUT_COOK_TORRANCE_FILE_NAME;
			const std::wstring lutTexturePath = ReflectionCapture::PRECOMPUTE_LUT_PATH / lutTextureFileName;
			std::shared_ptr<Texture2D> brdfLutMap = TextureManager::getInstance().getTexture2D(lutTexturePath);

			if (irradianceMap == nullptr)
			{
				D3D11_TEXTURE2D_DESC irradianceMapDesc{};
				irradianceMapDesc.Usage = D3D11_USAGE_DEFAULT;
				irradianceMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				irradianceMapDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

				irradianceMap = ReflectionCapture::getInstance().GetIrradianceMap(irradianceMapFileName, IRRADIANCE_MAP_BIND_SLOT, irradianceMapDesc);
			}
			if (prefilteredMap == nullptr)
			{
				D3D11_TEXTURE2D_DESC prefilteredMapDesc{};
				prefilteredMapDesc.Usage = D3D11_USAGE_DEFAULT;
				prefilteredMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				prefilteredMapDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;


				prefilteredMap = ReflectionCapture::getInstance().GetPrefilteredEnvMap(prefilteredMapFileName, PREFILTERED_MAP_BIND_SLOT, prefilteredMapDesc);
			}
			if (brdfLutMap == nullptr)
			{
				D3D11_TEXTURE2D_DESC brdfMapDesc{};
				brdfMapDesc.Usage = D3D11_USAGE_DEFAULT;
				brdfMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

				brdfLutMap = ReflectionCapture::getInstance().GetBRDFLookupTexture(lutTextureFileName, BRDF_LUT_BIND_SLOT, brdfMapDesc);
			}

			irradianceMap->bind();
			prefilteredMap->bind();
			brdfLutMap->bind();

			//set per frame buffer
			auto perFrameIBL = m_perFrameIBLbuffer.lock();
			perFrameIBL->updateData({ m_perFrameIBL });
			perFrameIBL->setPixelShaderBuffer();


		}

		RenderMode getRenderMode(const OpaqueInstances::ShaderGroup& shaderGroup);

		void setShaders(const OpaqueInstances::ShaderGroup& shaderGroup);

		void setPrimitiveTopology(const OpaqueInstances::ShaderGroup& shaderGroup);

	};



}
