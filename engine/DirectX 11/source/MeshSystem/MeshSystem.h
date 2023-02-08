#pragma once
#include "OpaqueInstances.h"


namespace engine::DX
{
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

		void render()
		{
			opaqueInstances->render();
		}

		void addOpaqueInstances(const std::shared_ptr<Model>& model, size_t meshIndex, const std::shared_ptr<OpaqueInstances::Material>& material, const std::vector<OpaqueInstances::Instance>& instances)
		{
			opaqueInstances->addInstances(model, meshIndex,  material, instances);
		}

		void setVertexShader(const std::wstring& fileName)
		{
			opaqueInstances->setVertexShader(fileName);
		}

		void setPixelShader(const std::wstring& fileName)
		{
			opaqueInstances->setPixelShader(fileName);
		}

	private:
		MeshSystem()
		{
			opaqueInstances = std::make_shared<OpaqueInstances>();
		};
		std::shared_ptr<OpaqueInstances> opaqueInstances;
	};

}
