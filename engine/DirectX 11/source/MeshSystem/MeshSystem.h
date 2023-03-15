#pragma once
#include "OpaqueInstances.h"
#include "../Math/Colision/ModelTriangleOctreeDX.h"
#include "../Controller/ModelManager.h"
#include "../../../general/utils/timer/FPSTimerRC.h"	 
#include "../Utils/Camera.h"

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

		void render(Camera& camera, bool showNormal);

		bool findIntersection(const ray& r, std::weak_ptr<Instance>& instance, Intersection& intersection);


		/// <param name="instance">instance, which is contained in opaqueInstance</param>
		void updateOpaqueInstanceBuffer(const std::weak_ptr<Instance>& instance);

		/// <param name="instances">instance may be change during dragging</param>
		void addInstances(uint32_t opaqueInstanceID,
			const std::shared_ptr<Model>& model,
			const std::vector<size_t>& meshIndex,
			const std::shared_ptr<OpaqueInstances::Material>& material,
			const std::vector<std::shared_ptr<OpaqueInstances::Instance>>& instances);

		/// <summary>
		/// creates new Opaque Instance
		/// </summary>
		/// <param name = "shaderFileNames">0 - vertex, 1 - hull shader, 2 - domain shader, 3 - geometry shader, 4 - pixel shader file names</param>
		/// <returns>ID to access to created opaque instance</returns>
		uint32_t createOpaqueInstance(const std::vector<std::array<std::wstring, 5>>& shaderFileNames);
		

	

		ConstantBuffer<DirectX::SimpleMath::Vector4> cameraPosition;
		ConstantBuffer<DirectX::SimpleMath::Vector4> time;

	private:
		MeshSystem()
		{
			cameraPosition.initBuffer(PER_VIEW_PIXEL_HOLOGRAM_SHADER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
			time.initBuffer(PER_DRAW_HOLOGRAM_SHADER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		};
		std::vector<std::shared_ptr<OpaqueInstances>> opaqueInstances{};
		std::vector<std::shared_ptr<ModelTriangleOctree>> triangleOctrees{};

		struct ModelIntersection
		{
			std::weak_ptr<ModelTriangleOctree> triangleOctree;
			std::weak_ptr<Instance> instance;
			std::weak_ptr<OpaqueInstances> opaqueInstance; // used to update instance buffer during dragging
		};

		std::vector<ModelIntersection> modelIntersections{};



	};

}
