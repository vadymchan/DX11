#pragma once
#include "OpaqueInstances.h"
#include "../Math/Colision/TriangleOctreeDX.h"


namespace engine::DX
{
	using Intersection = TriangleOctree::Intersection;
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

		bool findIntersection(const ray& r, Instance*& instance, Intersection& intersection);


		/// <param name="instance">instance, which is contained in opaqueInstance</param>
		void updateOpaqueInstanceBuffer(Instance* instance);




		/// <param name="instances">instance may be change during dragging</param>
		void addInstances(uint32_t opaqueInstanceID,
			const std::shared_ptr<Model>& model, size_t meshIndex,
			const std::shared_ptr<OpaqueInstances::Material>& material,
			const std::vector<OpaqueInstances::Instance>& instances);

		/// <summary>
		/// creates new Opaque Instance
		/// </summary>
		/// <returns>ID to access to created opaque instance</returns>
		uint32_t createOpaqueInstance(const std::wstring& vertexShaderFileName, const std::wstring& pixelShaderFileName);

	

		ConstantBuffer<DirectX::SimpleMath::Vector4> cameraPosition;
		ConstantBuffer<DirectX::SimpleMath::Vector4> time;

	private:
		MeshSystem()
		{
			cameraPosition.initBuffer(PER_VIEW_PIXEL_HOLOGRAM_SHADER, D3D11_USAGE_DYNAMIC, D3D10_CPU_ACCESS_WRITE);
			time.initBuffer(PER_DRAW_HOLOGRAM_SHADER, D3D11_USAGE_DYNAMIC, D3D10_CPU_ACCESS_WRITE);
		};
		std::vector<std::shared_ptr<OpaqueInstances>> opaqueInstances{};
		std::vector<std::shared_ptr<TriangleOctree>> triangleOctrees{};

		//used to update instance buffer during dragging
		std::unordered_map<Instance*, OpaqueInstances*> triangleOctreeInstances{};

	};

}
