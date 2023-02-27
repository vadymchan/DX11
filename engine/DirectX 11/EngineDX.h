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


		void initWindow(const LPCWSTR& title, int xStart, int yStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams)
		{
			window.initWindow(title, xStart, yStart, width, height, appHandle, windowShowParams);
		}



		/// <param name="fov">in degrees</param>
		void initCamera(const float3& position, const float3& direction, const float3& cameraUp, float fov, float aspect, float zNear, float zFar)
		{
			camera.setView(position, direction, cameraUp);
			camera.setPerspective(DirectX::XMConvertToRadians(fov), aspect, zNear, zFar);
			camera.initBuffer(PER_VIEW_SHADER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE); // may be moved in application
		}

		void addInstancedModel(uint32_t opaqueInstanceID, const std::shared_ptr<Model>& model, size_t meshIndex, const std::shared_ptr<OpaqueInstances::Material>& material, const std::vector<OpaqueInstances::Instance>& instances)
		{
			MeshSystem::getInstance().addInstances(opaqueInstanceID, model, meshIndex, material, instances);
		}

		uint32_t createOpaqueInstance(const std::wstring& vertexShaderFileName, const std::wstring& pixelShaderFileName)
		{
			return MeshSystem::getInstance().createOpaqueInstance(vertexShaderFileName, pixelShaderFileName);
		}

		void castRay()
		{
			for (int i = 0; i < window.GetViewport().Width; i += 10)
			{
				for (int j = 0; j < window.GetViewport().Height; j += 10)
				{
					castRay(i, j);
				}
				
			}
		}

		std::shared_ptr<OpaqueInstances::Material> mat = std::make_shared<OpaqueInstances::Material>(OpaqueInstances::Material{ float4{ 0,1,0,0 } });
		
		bool castRay(float xPos, float yPos)
		{
			Instance* instance = nullptr;
			ray ray = rayToWorld(xPos, yPos);
			Intersection intersection;
			intersection.reset();
			if (MeshSystem::getInstance().findIntersection(ray, instance, intersection))
			{

				MeshMover::getInstance().setIntersectionPoint(float4(intersection.point.x, intersection.point.y, intersection.point.z, 1));
				

				addInstancedModel(0, ModelManager::getInstance().loadCube1(), 0, mat,
					{ Instance { engine::DX::float4x4
				{{{0.01,0,0,intersection.point.x},
				{0,0.01,0,intersection.point.y},
				{0,0,0.01,intersection.point.z},
				{0,0,0,1},}}
				}});

			}
			bool returnResult{};
			if (instance != nullptr)
			{
				MeshMover::getInstance().setInstance(instance);
				returnResult = true;

			}


			return returnResult;
		}

		/// <param name="offset">offset in screen space (mouse offset)</param>
		void moveCapturedObject(const float3& offset)
		{



			float4 intersectionVec{ MeshMover::getInstance().getIntersectionPoint() - float4(camera.position().x, camera.position().y, camera.position().z, 1) };
			float nearPlaneOffsetX = 2 * tanf(camera.getFov() / 2) * camera.getZNear() * camera.getAspectRatio() * offset.x / window.GetViewport().Width;
			float nearPlaneOffsetY = 2 * tanf(camera.getFov() / 2) * camera.getZNear() * offset.y / window.GetViewport().Height;
			intersectionVec = float4::Transform(intersectionVec, camera.getViewMatrix().Transpose());
			float worldOffsetX = nearPlaneOffsetX * std::abs(intersectionVec.z) / camera.getZNear();
			float worldOffsetY = nearPlaneOffsetY * std::abs(intersectionVec.z) / camera.getZNear();
			float3 moveOffset = worldOffsetX * camera.right() + worldOffsetY * camera.up();
			//std::cout << "moveOffset: " << moveOffset.x << " " << moveOffset.y << " " << moveOffset.z << std::endl;
			//std::cout << nearPlaneOffsetX << " " << nearPlaneOffsetY << std::endl;
			//std::cout << worldOffsetX << " " << worldOffsetY << std::endl;
			//std::cout << offset.x << " " << offset.y << std::endl;
			//std::cout << intersectionVec.x << " " << intersectionVec.y << " " << intersectionVec.z << std::endl;



			//--------------------------------------------------------------------------------------------
			//transform translation to world space

			//float nearPlaneHeight = camera.getZNear() * tanf(DirectX::XMConvertToRadians(camera.getFov() / 2)) * 2;
			//float nearPlaneWidth = nearPlaneHeight * camera.getAspectRatio();

			//// window offset to world offset 
			//float xWorldOffset{ offset.x * nearPlaneWidth / window.GetViewport().Width };
			//float yWorldOffset{ offset.y * nearPlaneHeight / window.GetViewport().Height };


			////if (scene->mover != nullptr)
			////{
			////	//move paralel to camera
			////	if (cameraMove)
			////	{
			////		scene->mover->move(cameraDir * cameraSpeed * deltaTime);
			////	}
			//	//drag along plane
			//	/*else if (xOffset != 0 || yOffset != 0)
			//	{*/

			//		xWorldOffset = rayNearPlaneRatio * xWorldOffset;
			//		yWorldOffset = rayNearPlaneRatio * yWorldOffset;

			//		float3 moveOffset = xWorldOffset * camera.right() - yWorldOffset * camera.up();
			//	}
			//}

			//--------------------------------------------------------------------------------------------

			//viewport to NDC
			//float ndc_x = (2.0f * offset.x) / window.GetViewport().Width - 1.0f;
			//float ndc_y = 1.0f - (2.0f * offset.y) / window.GetViewport().Height; // flip the Y axis

			////NDC to view space

			//float focal_length = 1.0f / tanf(DirectX::XMConvertToRadians(camera.getFov() / 2.0f));
			//float ar = camera.getAspectRatio();
			//float3 ray_view(ndc_x / focal_length, (ndc_y * ar) / focal_length, 1.0f);


			//// Step 3 - intersect view vector with object Z plane (in view)

			//float4x4 worldMat = MeshMover::getInstance().getMat()->toWorldMatrix;
			//float4 f = float4::Transform(float4(worldMat._14, worldMat._24, worldMat._34, 0.0), camera.getViewMatrix());

			//float objViewPosZ = f.z;

			//float4 view_space_intersect = float4(ray_view.x * objViewPosZ, ray_view.y * objViewPosZ, ray_view.z * objViewPosZ, 1.0f);

			//// Step 4 - View to World space
			//float4x4 View = camera.getViewMatrix();
			//float4x4 InvView = View.Invert();
			//float4 point_world = float4::Transform(view_space_intersect, InvView);

			//perspective space to world space

			//MeshMover::getInstance().moveMesh(float3(point_world.x, point_world.y, point_world.z));
			//MeshMover::getInstance().moveMesh(float3(offset.x * 0.01, offset.y * 0.01, 0));
			MeshMover::getInstance().moveMesh(moveOffset);
			//MeshMover::getInstance().moveMesh(moveOffset);
		}


		void moveCamera(const float3& offset)
		{
			camera.addWorldOffset(offset);
		}

		void rotateCamera(const Angles& angles)
		{
			camera.setWorldAngles(angles);
		}

		const float3& getCameraForward() const
		{
			return camera.forward();
		}

		const float3& getCameraRight() const
		{
			return camera.right();
		}

		const float3& getCameraUp() const
		{
			return camera.up();
		}

		const float3& getCameraPosition() const
		{
			return camera.position();
		}

		void render()
		{
			checkAspectRatio();
			renderer.render(window, camera);
		}


	private:
		Renderer renderer;
		Window window;
		Camera camera;

		void checkAspectRatio()
		{
			float windowRatio{ window.GetAspectRatio() };
			if (windowRatio != camera.getAspectRatio())
			{
				camera.changeAspectRatio(windowRatio);
			}
		}

		ray rayToWorld(float xPos, float yPos)
		{

			float xNdc = (xPos * 2.0f / window.GetViewport().Width - 1);
			float yNdc = ((window.GetViewport().Height - yPos) * 2.0f / window.GetViewport().Height - 1);
			float4x4 IPV = camera.getIPV();

			// screen to ndc
			float4 nearPointNDC(xNdc, yNdc, 1.0f, 1.0f);
			float4 farPointNDC(xNdc, yNdc, 0.0f, 1.0f);

			//ndc to perspective to view to world

			float4 nearPointWorld = float4::Transform(nearPointNDC, IPV);
			float4 farPointWorld = float4::Transform(farPointNDC, IPV);
			nearPointWorld /= nearPointWorld.w;
			farPointWorld /= farPointWorld.w;


			float4 direction = farPointWorld - nearPointWorld;
			direction.Normalize();
			ray returnRay{ float3(nearPointWorld.x, nearPointWorld.y, nearPointWorld.z), float3(direction.x, direction.y, direction.z) };

			//std::cout << "ray: " << returnRay.position.x << " " << returnRay.position.y << " " << returnRay.position.z << std::endl;
			//std::cout << "ray: " << direction.x << " " << direction.y << " " << direction.z << std::endl;
			return returnRay;


			//viewport to NDC
			//float ndc_x = (2.0f * xPos) / window.GetViewport().Width - 1.0f;
			//float ndc_y = 1.0f - (2.0f * yPos) / window.GetViewport().Height; // flip the Y axis

			////NDC to view space
			//float3 viewSpace{ ndc_x, ndc_y, 1.0f };


			////projection matrix
			//float4x4 projectionMatrix{ camera.getPerspectiveMatrix() };
			//float4x4 projectionMatrixInverse{ projectionMatrix.Invert() };

			////view space to world space
			//float4x4 viewMatrix{ camera.getViewMatrix() };
			//float4x4 viewMatrixInverse{ viewMatrix.Invert() };
			//

			//float4 worldSpace{ float4::Transform(float4{ viewSpace.x, viewSpace.y, viewSpace.z, 1.0f }, viewMatrixInverse) };

			//return ray{ camera.position(), float3{ worldSpace.x, worldSpace.y, worldSpace.z } };
		}
	};
}



