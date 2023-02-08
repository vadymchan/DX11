#pragma once
#include "source/MeshSystem/MeshSystem.h"
#include "source/Controller/ModelManager.h"
#include "source/Controller/BufferManager.h"
#include "source/Controller/ShaderManager.h"
#include "source/MeshSystem/OpaqueInstances.h"
#include "window/WindowDX.h"
#include "source/Camera.h"
#include "source/Renderer.h"




namespace engine::DX
{
	//
	class Engine
	{
	public:
		

		void initWindow(const LPCWSTR& title, int xStart, int yStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams)
		{
			window.initWindow(title, xStart, yStart, width, height, appHandle, windowShowParams);
		}



		/// <param name="fov">in degrees</param>
		void initCamera(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction, const DirectX::SimpleMath::Vector3& cameraUp, float fov, float aspect, float zNear, float zFar)
		{
			camera.setView(position, direction, cameraUp);
			camera.setPerspective(DirectX::XMConvertToRadians(fov), aspect, zNear, zFar);
			camera.initBuffer(CONSTANT_REGISTER_0, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE); // may be moved in application
		}

		void addInstancedModel(const std::shared_ptr<Model>& model, size_t meshIndex, const std::shared_ptr<OpaqueInstances::Material>& material, const std::vector<OpaqueInstances::Instance>& instances)
		{
			MeshSystem::getInstance().addOpaqueInstances(model, meshIndex, material, instances);
		}

		void setVertexShader(const std::wstring& fileName)
		{
			MeshSystem::getInstance().setVertexShader(fileName);
		}

		void setPixelShader(const std::wstring& fileName)
		{
			MeshSystem::getInstance().setPixelShader(fileName);
		}

		void moveCamera(const DirectX::SimpleMath::Vector3& offset)
		{
			camera.addRelativeOffset(offset);
		}

		void rotateCamera(const Angles& angles)
		{
			camera.addRelativeAngles(angles);
		}

		const DirectX::SimpleMath::Vector3& getCameraForward()
		{
			return camera.forward();
		}

		const DirectX::SimpleMath::Vector3& getCameraRight()
		{
			return camera.right();
		}

		const DirectX::SimpleMath::Vector3& getCameraUp()
		{
			return camera.up();
		}

		
			
		void render()
		{
			renderer.render(window, camera);
		}


	private:
		Renderer renderer;
		Window window;
		Camera camera;
	};
}


