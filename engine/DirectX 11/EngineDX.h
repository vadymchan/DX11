#pragma once
#include "source/MeshSystem/MeshSystem.h"
#include "source/Controller/ModelManager.h"
#include "source/Controller/TextureManager.h"
#include "source/Controller/BufferManager.h"
#include "source/Controller/ShaderManager.h"
#include "source/MeshSystem/OpaqueInstances.h"
#include "source/Math/Movement/MeshMover.h"
#include "window/WindowDX.h"
#include "source/Utils/Camera.h"
#include "source/Renderer.h"



namespace engine::DX
{



		class Engine
		{
		public:


			void initWindow(const LPCSTR& title, int xStart, int yStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams);

			void initRenderer(const D3D11_RASTERIZER_DESC& rasterizerDesc);

			/// <param name="fov">in degrees</param>
			void initCamera(const float3& position, const float3& direction, const float3& cameraUp, float fov, float aspect, float zNear, float zFar);

			void addInstancedModel(uint32_t opaqueInstanceID, const std::shared_ptr<Model>& model, const std::vector<size_t>& meshIndex, const std::shared_ptr<OpaqueInstances::Material>& material, const std::vector<std::shared_ptr<OpaqueInstances::Instance>>& instances);



			/**
			* @param shaderFileNames std::array 0 - vertex, 1 - hull shader, 2 - domain shader, 3 - geometry shader, 4 - pixel shader file names
			*
			* @return an ID of opaque instance.
			*/
			uint32_t createOpaqueInstance(const std::vector<std::array<std::wstring, 5>>& shaderFileNames);

			void castRay();

			bool castRay(float xPos, float yPos);

			/// <param name="offset">offset in screen space (mouse offset)</param>
			void dragCapturedObject(float xPos, float yPos);

			void moveCapturedObject(const float3& offset);


			Camera& getCamera() { return camera; };

			Renderer& getRenderer() { return renderer; };

			void normalVisualization(bool isVisible)
			{
				renderer.setVisualizeNormal(isVisible);
			}

			void render();


		private:
			Renderer renderer;
			Window window;
			Camera camera;

			void checkAspectRatio();

			ray rayToWorld(float xPos, float yPos);
		};
	}



