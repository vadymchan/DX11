#pragma once
#include "source/MeshSystem/MeshSystem.h"
#include "source/Controller/ModelManager.h"
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


		void initWindow(const LPCWSTR& title, int xStart, int yStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams);

		void initRenderer(D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID, D3D11_CULL_MODE cullMode = D3D11_CULL_BACK, BOOL frontCounterClockwise = false, INT depthBias = 0, FLOAT depthBiasClamp = 0.0f, FLOAT slopeScaledDepthBias = 0.0f, BOOL depthClipEnable = true, BOOL scissorEnable = false, BOOL multisampleEnable = false, BOOL antialiasedLineEnable = false);

		/// <param name="fov">in degrees</param>
		void initCamera(const float3& position, const float3& direction, const float3& cameraUp, float fov, float aspect, float zNear, float zFar);

		void addInstancedModel(uint32_t opaqueInstanceID, const std::shared_ptr<Model>& model, const std::vector<size_t>& meshIndex, const std::shared_ptr<OpaqueInstances::Material>& material, const std::vector<std::shared_ptr<OpaqueInstances::Instance>>& instances);

		uint32_t createOpaqueInstance(const std::wstring& vertexShaderFileName, const std::wstring& pixelShaderFileName);

		void castRay();

		bool castRay(float xPos, float yPos);

		/// <param name="offset">offset in screen space (mouse offset)</param>
		void dragCapturedObject(float xPos, float yPos);

		void moveCapturedObject(const float3& offset);

		void moveCamera(const float3& offset);

		void rotateCamera(const Angles& angles);

		const float3& getCameraForward() ;

		const float3& getCameraRight() ;

		const float3& getCameraUp() ;

		const float3& getCameraPosition() ;

		void render();


	private:
		Renderer renderer;
		Window window;
		Camera camera;

		void checkAspectRatio();

		ray rayToWorld(float xPos, float yPos);
	};
}



