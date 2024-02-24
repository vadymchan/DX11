#pragma once
#include "Instance/OpaqueInstances.h"
#include "Instance/DissolutionInstances.h"
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

	class MeshSystem
	{
	public:

		enum class RenderInstance
		{
			Opaque,
			Dissolution,
			COUNT,
			All,
		};

		using DepthRender = RenderInstance;

		using OpaqueInstancesID = uint32_t;
		using DissolutionInstancesID = uint32_t;
		MeshSystem(const MeshSystem&) = delete;
		MeshSystem& operator=(const MeshSystem&) = delete;
		static MeshSystem& getInstance()
		{
			static MeshSystem instance;
			return instance;
		}

		template<RenderInstance Instance> 
		void render(Camera& camera) {}

		template<>
		void render<RenderInstance::Opaque>(Camera& camera)
		{
			renderHelper<OpaqueInstances>(camera, opaqueInstances);
		}

		template<>
		void render<RenderInstance::Dissolution>(Camera& camera)
		{
			renderHelper<DissolutionInstances>(camera, dissolutionInstances);
		}

		template<>
		void render<RenderInstance::All>(Camera& camera)
		{
			renderHelper<OpaqueInstances>(camera, opaqueInstances);
			renderHelper<DissolutionInstances>(camera, dissolutionInstances);
		}

		void renderUIPerFrameIBL()
		{
			ImGuiManager::getInstance().RenderCheckbox("useIBL", m_perFrameIBL.useIBL);
			if (m_perFrameIBL.useIBL)
			{
				ImGuiManager::getInstance().RenderCheckbox("useDiffuseReflection", m_perFrameIBL.useDiffuseReflection);
				ImGuiManager::getInstance().RenderCheckbox("useSpecularReflection", m_perFrameIBL.useSpecularReflection);
			}

			ImGuiManager::getInstance().RenderCheckbox("useRoughnessOverwriting", m_perFrameIBL.useRoughnessOverwriting);
			if (m_perFrameIBL.useRoughnessOverwriting)
			{
				ImGuiManager::getInstance().RenderSlider("overwrittenRoughnessValue", m_perFrameIBL.overwrittenRoughnessValue, 0.0f, 1.0f);
			}
		}


		template<DepthRender>
		void renderDepth2D() {}

		template<>
		void renderDepth2D<DepthRender::Opaque>()
		{
			const static std::wstring shadowMapVertexShaderFileName{ L"Shadow/2D/shadow2DVertexShader.hlsl" };
			const static std::wstring shadowMapPixelShaderFileName{ L"Shadow/2D/shadow2DPixelShader.hlsl" };

			static BaseInstances::ShaderGroup shaderGroup
			{
				BaseInstances::RenderType::SHADOW_GENERATION,
				{},
				ShaderManager::getInstance().getVertexShader(shadowMapVertexShaderFileName),
				{},
				{},
				{},
				ShaderManager::getInstance().getPixelShader(shadowMapPixelShaderFileName),
			};

			setShaders(shaderGroup);
			SetRenderMode<RenderMode::SHADOW_GENERATION>();
			setPrimitiveTopology(shaderGroup);
			for (const auto& opaqueInstance : opaqueInstances)
			{
				opaqueInstance->render();
			}


		}

		template<>
		void renderDepth2D<DepthRender::Dissolution>()
		{
			const std::wstring shadowMapVertexShaderFileName{ L"Shadow/2D/shadow2DDissolutionVertexShader.hlsl" };
			const std::wstring shadowMapPixelShaderFileName{ L"Shadow/2D/shadow2DDissolutionPixelShader.hlsl" };

			static BaseInstances::ShaderGroup shaderGroup
			{
				BaseInstances::RenderType::SHADOW_GENERATION,
				{},
				ShaderManager::getInstance().getVertexShader(shadowMapVertexShaderFileName),
				{},
				{},
				{},
				ShaderManager::getInstance().getPixelShader(shadowMapPixelShaderFileName),
			};

			setShaders(shaderGroup);
			SetRenderMode<RenderMode::SHADOW_GENERATION>();
			setPrimitiveTopology(shaderGroup);

			for (const auto& dissolutionInstances : dissolutionInstances)
			{
				dissolutionInstances->render();
			}
		}


		template<DepthRender>
		void renderDepthCubemap() {}

		template<>
		void renderDepthCubemap<DepthRender::Opaque>()
		{
			const static std::wstring shadowCubeMapVertexShaderFileName{ L"Shadow/Omnidirectional/shadow3DVertexShader.hlsl" };
			const static std::wstring shadowCubeMapGeometryShaderFileName{ L"Shadow/Omnidirectional/shadow3DGeometryShader.hlsl" };
			const static std::wstring shadowCubeMapPixelShaderFileName{ L"Shadow/Omnidirectional/shadow3DPixelShader.hlsl" };


			static BaseInstances::ShaderGroup shaderGroup
			{
				BaseInstances::RenderType::SHADOW_GENERATION,
				{},
				ShaderManager::getInstance().getVertexShader(shadowCubeMapVertexShaderFileName),
				{},
				{},
				ShaderManager::getInstance().getGeometryShader(shadowCubeMapGeometryShaderFileName),
				ShaderManager::getInstance().getPixelShader(shadowCubeMapPixelShaderFileName),
			};

			setShaders(shaderGroup);
			SetRenderMode<RenderMode::SHADOW_GENERATION>();
			setPrimitiveTopology(shaderGroup);

			for (const auto& opaqueInstance : opaqueInstances)
			{

				opaqueInstance->render();
			}

		}

		template<>
		void renderDepthCubemap<DepthRender::Dissolution>()
		{
			const static std::wstring shadowCubeMapVertexShaderFileName{ L"Shadow/Omnidirectional/shadow3DDissolutionVertexShader.hlsl" };
			const static std::wstring shadowCubeMapGeometryShaderFileName{ L"Shadow/Omnidirectional/shadow3DDissolutionGeometryShader.hlsl" };
			const static std::wstring shadowCubeMapPixelShaderFileName{ L"Shadow/Omnidirectional/shadow3DDissolutionPixelShader.hlsl" };


			static BaseInstances::ShaderGroup shaderGroup
			{
				BaseInstances::RenderType::SHADOW_GENERATION,
				{},
				ShaderManager::getInstance().getVertexShader(shadowCubeMapVertexShaderFileName),
				{},
				{},
				ShaderManager::getInstance().getGeometryShader(shadowCubeMapGeometryShaderFileName),
				ShaderManager::getInstance().getPixelShader(shadowCubeMapPixelShaderFileName),
			};

			setShaders(shaderGroup);
			SetRenderMode<RenderMode::SHADOW_GENERATION>();
			setPrimitiveTopology(shaderGroup);

			for (const auto& dissolutionInstances : dissolutionInstances)
			{
				dissolutionInstances->render();
			}
		}


		

		bool findIntersection(const ray& r, TransformSystem::ID& instance, RayIntersection& intersection);

		/// <param name="instance">instance, which is contained in instanceGroup</param>
		void updateInstanceBuffer(const TransformSystem::ID instance);

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
		void addOpaqueInstances(OpaqueInstancesID opaqueInstanceID,
			const std::shared_ptr<Model>& model,
			const std::vector<size_t>& meshIndex,
			const std::shared_ptr<OpaqueInstances::Material>& material,
			const std::vector<OpaqueInstances::Instance>& transformID);

		/// <param name="instances">instance may be change during dragging</param>
		void addDissolutionInstances(DissolutionInstancesID opaqueInstanceID,
			const std::shared_ptr<Model>& model,
			const std::vector<size_t>& meshIndex,
			const std::shared_ptr<DissolutionInstances::Material>& material,
			const std::vector<std::shared_ptr<DissolutionInstances::Instance>>& instances);

		std::shared_ptr<DissolutionInstances::Instance> getDissolutionInstance(DissolutionInstancesID dissolutionInstancesID, const DissolutionInstances::ModelInstanceID& id)
		{
			return  dissolutionInstances[dissolutionInstancesID]->getDissolutionInstance(id);
		}

		void removeDissolutionInstance(DissolutionInstancesID dissolutionInstancesID, const DissolutionInstances::ModelInstanceID& dissolutionModelInstanceID)
		{
			dissolutionInstances[dissolutionInstancesID]->removeModelInstance(dissolutionModelInstanceID);
		}

		/// <summary>
		/// creates new Opaque Instance
		/// </summary>
		/// <param name = "shaderFileNames">0 - vertex, 1 - hull shader, 2 - domain shader, 3 - geometry shader, 4 - pixel shader file names</param>
		/// <returns>ID to access to created opaque instance</returns>
		OpaqueInstancesID createOpaqueInstance(const std::vector<std::array<std::wstring, (int)OpaqueInstances::ShaderType::SHADER_TYPE_NUM>>& shaderFileNames);

		/// <summary>
		/// creates new Dissolution Instance
		/// </summary>
		/// <param name = "shaderFileNames">0 - vertex, 1 - hull shader, 2 - domain shader, 3 - geometry shader, 4 - pixel shader file names</param>
		/// <returns>ID to access to created Dissolution instance</returns>
		DissolutionInstancesID createDissolutionInstance(const std::vector<std::array<std::wstring, (int)DissolutionInstances::ShaderType::SHADER_TYPE_NUM>>& shaderFileNames);




		ConstantBuffer<DirectX::SimpleMath::Vector4> cameraPosition;
		ConstantBuffer<DirectX::SimpleMath::Vector4> time;

	private:

		template <typename T>
		void renderHelper(Camera& camera, std::vector<std::shared_ptr<T>> instances)
		{
			for (const auto& instance : instances)
			{
				//bind shaders (draw several times with different shader)
				for (auto& shaderGroup : instance->getShaders())
				{
					RenderMode renderModeType = getRenderMode(shaderGroup);

					setShaders(shaderGroup);

					switch (renderModeType)
					{
					case RenderMode::DEFAULT:
						SetRenderMode<RenderMode::DEFAULT>(&camera);
						break;
					case RenderMode::NORMAL_VISUALISER:
						if (!m_showNormal)
							continue;
						SetRenderMode<RenderMode::NORMAL_VISUALISER>(&camera);
						break;
					case RenderMode::HOLOGRAM:
						SetRenderMode<RenderMode::HOLOGRAM>(&camera);
						break;
					case RenderMode::BLINN_PHONG:
						SetRenderMode<RenderMode::BLINN_PHONG>(&camera);
						break;
					case RenderMode::POINT_SPHERE:
						SetRenderMode<RenderMode::POINT_SPHERE>(&camera);
						break;
					case RenderMode::PBR:
						SetRenderMode<RenderMode::PBR>(&camera);
						break;
					case RenderMode::IBL_SHADOW:
						SetRenderMode<RenderMode::IBL_SHADOW>(&camera);
						break;
					case RenderMode::IBL:
						SetRenderMode<RenderMode::IBL>(&camera);
						break;
					}

					setPrimitiveTopology(shaderGroup);

					instance->render();
				}
			}
		}


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

		//std::array<>

		std::vector<std::shared_ptr<OpaqueInstances>> opaqueInstances{};
		std::vector<std::shared_ptr<DissolutionInstances>> dissolutionInstances{};
		std::vector<std::shared_ptr<ModelTriangleOctree>> triangleOctrees{};
		std::weak_ptr<Skybox> currentSkybox; // used to take skybox for taking the right environment map

		bool m_showNormal{};
		std::weak_ptr<ConstantBuffer<PerFrameIBL>> m_perFrameIBLbuffer;
		PerFrameIBL m_perFrameIBL;

		struct ModelIntersection
		{
			std::weak_ptr<ModelTriangleOctree> triangleOctree;
			TransformSystem::ID instance;
			std::weak_ptr<BaseInstances> instanceGroup; // used to update instance buffer during dragging
		};

		std::vector<ModelIntersection> modelIntersections{};

#pragma region RenderMode

		enum class RenderMode
		{
			DEFAULT,
			NORMAL_VISUALISER,
			HOLOGRAM,
			BLINN_PHONG,
			POINT_SPHERE,
			PBR,
			IBL,
			SHADOW_GENERATION,
			IBL_SHADOW,
			DISSOLUTION_SHADOW_GENERATION,
			IBL_DISSOLUTION
		};

		template<RenderMode Mode>
		void SetRenderMode(Camera* camera = nullptr) {};

		template<>
		void SetRenderMode<RenderMode::HOLOGRAM>( Camera* camera)
		{

			time.setBufferData(std::vector<DirectX::SimpleMath::Vector4>{ {(general::FPSTimer::getCurrentTick() - general::FPSTimer::initTick) * general::FPSTimer::milliToSec, 0, 0, 0}});
			time.setBuffer();

			float3 cameraPos = camera->position();
			cameraPosition.setBufferData(std::vector<DirectX::SimpleMath::Vector4>{ {cameraPos.x, cameraPos.y, cameraPos.z, 1.0}});
			cameraPosition.setPixelShaderBuffer();
			camera->setCameraBufferGeometryShader();
		}


		template<>
		void SetRenderMode<RenderMode::IBL_DISSOLUTION>(Camera* camera)
		{
			SetRenderMode<RenderMode::IBL>(camera);

			LightSystem::getInstance().bindShadowMapTextures();
		}

		template<>
		void SetRenderMode<RenderMode::IBL_SHADOW>( Camera* camera)
		{
			SetRenderMode<RenderMode::IBL>(camera);

			LightSystem::getInstance().bindShadowMapTextures();
		}

		//set textures to pixel shader (irradiance, prefilteredEnvMap, brdfLUT)
		template<>
		void SetRenderMode<RenderMode::IBL>( Camera* camera)
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

		RenderMode getRenderMode(const BaseInstances::ShaderGroup& shaderGroup);

		void setShaders(const BaseInstances::ShaderGroup& shaderGroup);

		void setPrimitiveTopology(const BaseInstances::ShaderGroup& shaderGroup);

#pragma endregion

	};



}
