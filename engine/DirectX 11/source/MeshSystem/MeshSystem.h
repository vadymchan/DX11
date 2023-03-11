#pragma once
#include "OpaqueInstances.h"
#include "../Math/Colision/ModelTriangleOctreeDX.h"
#include "../Controller/ModelManager.h"

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

		void render(const DirectX::SimpleMath::Vector3& cameraPos = {});

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
		/// <returns>ID to access to created opaque instance</returns>
		uint32_t createOpaqueInstance(const std::wstring& vertexShaderFileName, const std::wstring& pixelShaderFileName, const std::wstring& geometryShaderFileName = L"");

	

		ConstantBuffer<DirectX::SimpleMath::Vector4> cameraPosition;
		ConstantBuffer<DirectX::SimpleMath::Vector4> time;

	private:
		MeshSystem()
		{
			cameraPosition.initBuffer(PER_VIEW_PIXEL_HOLOGRAM_SHADER, D3D11_USAGE_DYNAMIC, D3D10_CPU_ACCESS_WRITE);
			time.initBuffer(PER_DRAW_HOLOGRAM_SHADER, D3D11_USAGE_DYNAMIC, D3D10_CPU_ACCESS_WRITE);
		};
		std::vector<std::shared_ptr<OpaqueInstances>> opaqueInstances{};
		std::vector<std::shared_ptr<ModelTriangleOctree>> triangleOctrees{};


		//std::vector<std::pair<std::weak_ptr<Instance>, OpaqueInstances*>> triangleOctreeInstances{};

		struct ModelIntersection
		{
			std::weak_ptr<ModelTriangleOctree> triangleOctree;
			std::weak_ptr<Instance> instance;
			std::weak_ptr<OpaqueInstances> opaqueInstance; // used to update instance buffer during dragging
		};

		std::vector<ModelIntersection> modelIntersections{};

	};

}
