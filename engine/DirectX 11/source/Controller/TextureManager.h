#pragma once
#include <unordered_map>
#include "../Texture/Texture2D.h"
#include "../Texture/DDSTextureLoader11.h"
#include "../D3D/D3D.h"

namespace engine::DX
{
	const std::wstring textureDirectory = L"engine\\general\\resources\\textures\\";


	class TextureManager
	{
	public:

		static TextureManager& getInstance()
		{
			static TextureManager instance;
			return instance;
		}


		void addTexture2D(
			const std::wstring& fileName,
			const D3D11_TEXTURE2D_DESC& textureDesc,
			size_t maxSize = 0,
			DirectX::DDS_LOADER_FLAGS loadFlags = DirectX::DDS_LOADER_DEFAULT,
			DirectX::DDS_ALPHA_MODE* alphaMode = nullptr)
		{
			assert(m_2dTextures.find(fileName) == m_2dTextures.end() && "Texture is already created! Error");
			/*DirectX::CreateDDSTextureFromFileEx(g_device, g_devcon, fileName.c_str(), 0, )*/

			

			
		}

	private:
		TextureManager() = default;
		TextureManager(const TextureManager&) = delete;
		TextureManager& operator=(const TextureManager&) = delete;

		std::unordered_map<std::wstring, Texture2D> m_2dTextures{};
		
		
	};

}