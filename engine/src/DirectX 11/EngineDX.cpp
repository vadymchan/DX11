#include "EngineDX.h"

namespace engine::DX
{



		void Engine::initWindow(const LPCSTR& title, int xStart, int yStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams)
		{
			window.initWindow(title, xStart, yStart, width, height, appHandle, windowShowParams);
		}

		void Engine::initRenderer(const D3D11_RASTERIZER_DESC& rasterizerDesc, const std::shared_ptr<Skybox>& skybox)
		{
			renderer.initRasterizator(rasterizerDesc, skybox);
		}

		/// <param name="fov">in degrees</param>


		void Engine::initCamera(const float3& position, const float3& direction, const float3& worldUp, float fov, float aspect, float zNear, float zFar)
		{
			camera.setView(position, direction, worldUp);
			camera.setPerspective(DirectX::XMConvertToRadians(fov), aspect, zNear, zFar);
			camera.initBuffer(PER_VIEW_SHADER, INV_PER_VIEW_SHADER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE); // may be moved in application
		}

		std::vector<TransformSystem::ID> Engine::addOpaqueInstances(
			MeshSystem::OpaqueInstancesID opaqueInstancesID,
			const std::shared_ptr<Model>& model,
			const std::vector<size_t>& meshIndices,
			const std::shared_ptr<OpaqueInstances::Material>& material,
			const std::vector<float4x4>& instanceMatrices,
			const std::vector<float4>& color)
		{
			assert((color.empty() || instanceMatrices.size() == color.size()) && "Size of instances and color should be the same");

			std::vector<TransformSystem::ID> instanceMatrixIDs;
			instanceMatrixIDs.reserve(instanceMatrices.size());
			std::vector<OpaqueInstances::Instance> instances;
			instances.resize(instanceMatrices.size());
			for (size_t i = 0; i < instanceMatrices.size(); i++)
			{
				OpaqueInstances::Instance instance;
				instance.color = color.empty() ? float4{ -1,-1,-1,-1 } : color[i];
				instance.worldMatrixID = instanceMatrixIDs.emplace_back(
					TransformSystem::getInstance().addTransform(instanceMatrices[i]));
				instances[i] = instance;
			}

			MeshSystem::getInstance().addOpaqueInstances(opaqueInstancesID, model, meshIndices, material, instances);
			return instanceMatrixIDs;
		}

		void Engine::addOpaqueInstances(MeshSystem::OpaqueInstancesID opaqueInstancesID, const std::shared_ptr<Model>& model, const std::vector<size_t>& meshIndices, const std::shared_ptr<OpaqueInstances::Material>& material, const std::vector<TransformSystem::ID>& transformInstanceIDs)
		{
			
			std::vector<OpaqueInstances::Instance> instances;
			instances.resize(transformInstanceIDs.size());
			for (size_t i = 0; i < transformInstanceIDs.size(); i++)
			{
				OpaqueInstances::Instance instance;
				instance.color =float4{ -1,-1,-1,-1 } ;
				instance.worldMatrixID = transformInstanceIDs[i];
				instances[i] = instance;
			}

			MeshSystem::getInstance().addOpaqueInstances(opaqueInstancesID, model, meshIndices, material, instances);
		}

		std::vector<DissolutionInstances::ModelInstanceID> Engine::addDissolutionInstances(
			MeshSystem::DissolutionInstancesID opaqueInstanceID,
			const std::shared_ptr<Model>& model,
			const std::vector<size_t>& meshIndices,
			const std::shared_ptr<DissolutionInstances::Material>& material,
			const std::vector<float4x4>& instanceMatrices,
			const std::vector<float>& time)
		{
			assert((time.empty() || instanceMatrices.size() == time.size()) && "Size of instances and time should be the same");

			std::vector<DissolutionInstances::ModelInstanceID> modelInstanceIDs;
			modelInstanceIDs.reserve(instanceMatrices.size());
			std::vector<std::shared_ptr<DissolutionInstances::Instance>> instances;
			instances.resize(instanceMatrices.size());
			for (size_t i = 0; i < instanceMatrices.size(); i++)
			{
				auto instance = std::make_shared<DissolutionInstances::Instance>(DissolutionInstances::Instance{ TransformSystem::getInstance().addTransform(instanceMatrices[i]), time.empty() ? 0.0f : time[i] });
				modelInstanceIDs.emplace_back(DissolutionInstances::ModelInstanceID{ model, meshIndices, material, instance->worldMatrixID });
				instances[i] = instance;
			}

			MeshSystem::getInstance().addDissolutionInstances(opaqueInstanceID, model, meshIndices, material, instances);
			return modelInstanceIDs;
		}

		std::shared_ptr<DissolutionInstances::Instance> Engine::getDissolutionInstance(MeshSystem::DissolutionInstancesID dissolutionInstancesID, const DissolutionInstances::ModelInstanceID& dissolutionModelInstanceID)
		{
			return MeshSystem::getInstance().getDissolutionInstance(dissolutionInstancesID, dissolutionModelInstanceID);
		}

		void Engine::removeDissolutionInstance(MeshSystem::DissolutionInstancesID dissolutionInstancesID, const DissolutionInstances::ModelInstanceID& dissolutionModelInstanceID)
		{
			MeshSystem::getInstance().removeDissolutionInstance(dissolutionInstancesID, dissolutionModelInstanceID);
			//TransformSystem::getInstance().removeTransform(dissolutionModelInstanceID.transformID); - we should also remove the transform matrices but for optimization (removing from dissolution transform and then add opaue transform instances) we don't do th
		}


		uint32_t Engine::createOpaqueInstance(const std::vector<std::array<std::wstring, 5>>& shaderFileNames)
		{
			return MeshSystem::getInstance().createOpaqueInstance(shaderFileNames);
		}

		uint32_t Engine::createDissolutionInstance(const std::vector<std::array<std::wstring, (int)DissolutionInstances::ShaderType::SHADER_TYPE_NUM>>& shaderFileNames)
		{
			return MeshSystem::getInstance().createDissolutionInstance(shaderFileNames);
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
			std::shared_ptr<OpaqueInstances::Material> diamond = std::make_shared<OpaqueInstances::Material>(OpaqueInstances::Material{ L"diamond" });
#endif // color of intersection point

			ray ray = rayToWorld(xPos, yPos);
			TransformSystem::ID instance{ UINT32_MAX };
			RayIntersection intersection;
			intersection.reset();

			bool modelIntersection{ MeshSystem::getInstance().findIntersection(ray, instance, intersection) };
			bool particlesIntersection {particleSystem.findIntersection(ray, instance, intersection)};

			bool returnResult{modelIntersection || particlesIntersection};
			if (returnResult)
			{
				MeshMover::getInstance().setIntersectionPoint(float4(intersection.point.x, intersection.point.y, intersection.point.z, 1));
				MeshMover::getInstance().setMeshInstance(instance);

#ifdef INTERSECTION_TEST
				addInstancedModel(0, ModelManager::getInstance().getModel(ModelManager::debugCubeTag), { 0 }, diamond,
					{ engine::DX::float4x4
					{ { { 0.01,0,0,intersection.point.x},
					{ 0,0.01,0,intersection.point.y},
					{ 0,0,0.01,intersection.point.z},
					{ 0,0,0,1 }, } } });
#endif // add intersection point
			}

			
			return returnResult;
		}

		/// <param name="offset">offset in screen space (mouse offset)</param>


		void Engine::dragCapturedObject(float xScreenOffset, float yScreenOffset)
		{


			float3 cameraPos = camera.position();
			float4 intersectionVec{ MeshMover::getInstance().getIntersectionPoint() - float4(cameraPos.x, cameraPos.y, cameraPos.z, 1) };
			float nearPlaneOffsetX = 2 * tanf(camera.getFov() / 2) * camera.getZNear() * camera.getAspectRatio() * xScreenOffset / window.GetViewport().Width;
			float nearPlaneOffsetY = 2 * tanf(camera.getFov() / 2) * camera.getZNear() * yScreenOffset / window.GetViewport().Height;
			intersectionVec = float4::Transform(intersectionVec, camera.getViewMatrix());
			float worldOffsetX = nearPlaneOffsetX * intersectionVec.z / camera.getZNear();
			float worldOffsetY = nearPlaneOffsetY * intersectionVec.z / camera.getZNear();
			float3 moveOffset = worldOffsetX * camera.right() + worldOffsetY * camera.up();

			DirectX::SimpleMath::Matrix worldMatrix = TransformSystem::getInstance().getTransform(MeshMover::getInstance().getMat());
			DirectX::SimpleMath::Vector3 worldSpaceScale;

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
