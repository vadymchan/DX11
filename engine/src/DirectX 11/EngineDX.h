#pragma once
#include "source/MeshSystem/MeshSystem.h"
#include "source/Light/LightSystem.h"
#include "source/Controller/ModelManager.h"
#include "source/Controller/TextureManager.h"
#include "source/Controller/BufferManager.h"
#include "source/Controller/ShaderManager.h"
#include "source/MeshSystem/Instance/OpaqueInstances.h"
#include "source/MeshSystem/Instance/DissolutionInstances.h"
#include "source/Math/Movement/MeshMover.h"
#include "window/WindowDX.h"
#include "source/Utils/Camera.h"
#include "source/Render/Renderer.h"
#include "source/Math/Integration/FibonacciPointDistribution.h"
#include "source/Texture/ReflectionCapture.h"
#include "../general/utils/console/Console.h"
#include "source/ParticleSystem/ParticleSystem.h"


namespace engine::DX
{



	class Engine
	{
	public:



		void initWindow(const LPCSTR& title, int xStart, int yStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams);

		void initRenderer(const D3D11_RASTERIZER_DESC& rasterizerDesc, const std::shared_ptr<Skybox>& skybox = std::shared_ptr<Skybox>());

		/// <param name="fov">in degrees</param>
		void initCamera(const float3& position, const float3& direction, const float3& cameraUp, float fov, float aspect, float zNear, float zFar);

		

		std::vector<TransformSystem::ID> addOpaqueInstances(
			MeshSystem::OpaqueInstancesID opaqueInstancesID,
			const std::shared_ptr<Model>& model,
			const std::vector<size_t>& meshIndex,
			const std::shared_ptr<OpaqueInstances::Material>& material,
			const std::vector<float4x4>& instanceMatrices,
			const std::vector<float4>& color = {});

		void addOpaqueInstances(
			MeshSystem::OpaqueInstancesID opaqueInstancesID,
			const std::shared_ptr<Model>& model,
			const std::vector<size_t>& meshIndex,
			const std::shared_ptr<OpaqueInstances::Material>& material,
			const std::vector<TransformSystem::ID>& transformInstanceID);

		std::vector<DissolutionInstances::ModelInstanceID> addDissolutionInstances(
			MeshSystem::DissolutionInstancesID dissolutionInstancesID,
			const std::shared_ptr<Model>& model,
			const std::vector<size_t>& meshIndex,
			const std::shared_ptr<DissolutionInstances::Material>& material,
			const std::vector<float4x4>& instanceMatrices,
			const std::vector<float>& time = {});

		std::shared_ptr<DissolutionInstances::Instance> getDissolutionInstance(
			MeshSystem::DissolutionInstancesID dissolutionInstancesID, 
			const DissolutionInstances::ModelInstanceID& dissolutionModelInstanceID);

		void removeDissolutionInstance(
			MeshSystem::DissolutionInstancesID dissolutionInstancesID,
			const DissolutionInstances::ModelInstanceID& dissolutionModelInstanceID);

		/**
		* @param shaderFileNames std::array 0 - vertex, 1 - hull shader, 2 - domain shader, 3 - geometry shader, 4 - pixel shader file names
		*
		* @return an ID of opaque instance.
		*/
		uint32_t createOpaqueInstance(const std::vector<std::array<std::wstring, (int)OpaqueInstances::ShaderType::SHADER_TYPE_NUM>>& shaderFileNames);

		uint32_t createDissolutionInstance(const std::vector<std::array<std::wstring, (int)DissolutionInstances::ShaderType::SHADER_TYPE_NUM>>& shaderFileNames);

		void castRay();

		bool castRay(float xPos, float yPos);

		/// <param name="offset">offset in screen space (mouse offset)</param>
		void dragCapturedObject(float xPos, float yPos);

		void moveCapturedObject(const float3& offset);

		Camera& getCamera() { return camera; };

		Renderer& getRenderer() { return renderer; };

		Window& getWindow() { return window; };

		ParticleSystem& getParticleSystem() { return particleSystem; };

		void normalVisualization(bool isVisible)
		{
			renderer.setVisualizeNormal(isVisible);
		}

		void render();

		Engine()
		{
#ifdef _DEBUG
			general::initConsole();
#endif
			D3D::Init();

			LightSystem::getInstance().setCamera(camera);

			renderer.setParticleSystem(&particleSystem);
			//particleSystem.setCamera(camera);
		}

	private:
		Renderer renderer;
		Window window;
		Camera camera;

		ParticleSystem particleSystem;


		void checkAspectRatio();

		ray rayToWorld(float xPos, float yPos);
	};
}



