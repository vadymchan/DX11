#include "EngineDX.h"

namespace engine::DX
{



		void Engine::initWindow(const LPCSTR& title, int xStart, int yStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams)
		{
			window.initWindow(title, xStart, yStart, width, height, appHandle, windowShowParams);
		}

		void Engine::initRenderer(const D3D11_RASTERIZER_DESC& rasterizerDesc, const std::wstring& skyboxTextureFileName)
		{
			renderer.initRasterizator(rasterizerDesc, skyboxTextureFileName);
		}

		/// <param name="fov">in degrees</param>


		void Engine::initCamera(const float3& position, const float3& direction, const float3& cameraUp, float fov, float aspect, float zNear, float zFar)
		{
			camera.setView(position, direction, cameraUp);
			camera.setPerspective(DirectX::XMConvertToRadians(fov), aspect, zNear, zFar);
			camera.initBuffer(PER_VIEW_SHADER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE); // may be moved in application
		}

		void Engine::addInstancedModel(uint32_t opaqueInstanceID, const std::shared_ptr<Model>& model, const std::vector<size_t>& meshIndices, const std::shared_ptr<OpaqueInstances::Material>& material, const std::vector<std::shared_ptr<OpaqueInstances::Instance>>& instances)
		{
			MeshSystem::getInstance().addInstances(opaqueInstanceID, model, meshIndices, material, instances);
		}


		uint32_t Engine::createOpaqueInstance(const std::vector<std::array<std::wstring, 5>>& shaderFileNames)
		{
			return MeshSystem::getInstance().createOpaqueInstance(shaderFileNames);
		}

		void Engine::castRay()
		{
			for (int i = 0; i < window.GetViewport().Width; i += 10)
			{
				for (int j = 0; j < window.GetViewport().Height; j += 10)
				{
					castRay(i, j);
				}

			}
		}

		bool Engine::castRay(float xPos, float yPos)
		{
#ifdef INTERSECTION_TEST
			std::shared_ptr<OpaqueInstances::Material> mat = std::make_shared<OpaqueInstances::Material>(OpaqueInstances::Material{ float4{ 0,1,0,0 } });
#endif // color of intersection point

			ray ray = rayToWorld(xPos, yPos);
			std::weak_ptr<Instance> instance;
			Intersection intersection;
			intersection.reset();
			if (MeshSystem::getInstance().findIntersection(ray, instance, intersection))
			{

				MeshMover::getInstance().setIntersectionPoint(float4(intersection.point.x, intersection.point.y, intersection.point.z, 1));
				std::cout << intersection.point.x << " " << intersection.point.y << " " << intersection.point.z << std::endl;

#ifdef INTERSECTION_TEST
				addInstancedModel(0, ModelManager::getInstance().getModel(ModelManager::debugCubeTag), { 0 }, mat,
					{ std::make_shared<Instance>(Instance {engine::DX::float4x4
					{ { { 0.01,0,0,intersection.point.x},
					{ 0,0.01,0,intersection.point.y},
					{ 0,0,0.01,intersection.point.z},
					{ 0,0,0,1 }, } }
					}) });
#endif // add intersection point



			}
			bool returnResult{};
			if (!instance.expired())
			{
				MeshMover::getInstance().setInstance(instance);
				returnResult = true;

			}


			return returnResult;
		}

		/// <param name="offset">offset in screen space (mouse offset)</param>


		void Engine::dragCapturedObject(float xPos, float yPos)
		{


			float3 cameraPos = camera.position();
			float4 intersectionVec{ MeshMover::getInstance().getIntersectionPoint() - float4(cameraPos.x, cameraPos.y, cameraPos.z, 1) };
			float nearPlaneOffsetX = 2 * tanf(camera.getFov() / 2) * camera.getZNear() * camera.getAspectRatio() * xPos / window.GetViewport().Width;
			float nearPlaneOffsetY = 2 * tanf(camera.getFov() / 2) * camera.getZNear() * yPos / window.GetViewport().Height;
			intersectionVec = float4::Transform(intersectionVec, camera.getViewMatrix());
			float worldOffsetX = nearPlaneOffsetX * intersectionVec.z / camera.getZNear();
			float worldOffsetY = nearPlaneOffsetY * intersectionVec.z / camera.getZNear();
			float3 moveOffset = worldOffsetX * camera.right() + worldOffsetY * camera.up();

			DirectX::SimpleMath::Matrix worldMatrix = engine::DX::MeshMover::getInstance().getMat().lock()->toWorldMatrix;
			DirectX::SimpleMath::Vector3 worldSpaceScale;
			worldSpaceScale.x = worldMatrix.Right().Length();
			worldSpaceScale.y = worldMatrix.Up().Length();
			worldSpaceScale.z = worldMatrix.Backward().Length();
			moveOffset *= worldSpaceScale;
			MeshMover::getInstance().moveMesh(moveOffset);
		}

		void Engine::moveCapturedObject(const float3& offset)
		{
			MeshMover::getInstance().moveMesh(offset);
		}

		void Engine::render()
		{
			checkAspectRatio();
			renderer.render(window, camera);
		}

		void Engine::checkAspectRatio()
		{
			float windowRatio{ window.GetAspectRatio() };
			if (windowRatio != camera.getAspectRatio())
			{
				camera.changeAspectRatio(windowRatio);
			}
		}

		ray Engine::rayToWorld(float xPos, float yPos)
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

			return returnRay;

		}
	}
