#pragma once
#include <unordered_map>
#include "../Texture/Texture2D.h"
#include "../Texture/DDSTextureLoader11.h"
#include "../D3D/D3D.h"
#include <filesystem>
#include <DirectXTex.h>

namespace engine
{
	namespace DX
	{
		const UINT COLOR_TEXTURE_BIND_SLOT = 0;
		const UINT ALBEDO_TEXTURE_BIND_SLOT = 0;
		const UINT ROUGHNESS_TEXTURE_BIND_SLOT = 1;
		const UINT METALNESS_TEXTURE_BIND_SLOT = 2;
		const UINT NORMAL_TEXTURE_BIND_SLOT = 3;
		const UINT IRRADIANCE_MAP_BIND_SLOT = 4;
		const UINT PREFILTERED_MAP_BIND_SLOT = 5;
		const UINT BRDF_LUT_BIND_SLOT = 6;
		const UINT FLASHLIGHT_TEXTURE_BIND_SLOT = 7;
		const UINT DIRECTIONAL_LIGHT_SHADOW_MAPS_SLOT = 8;
		const UINT SPOT_LIGHT_SHADOW_MAPS_SLOT = 9;
		const UINT POINT_LIGHT_SHADOW_MAPS_SLOT = 10;
		const UINT FLASH_LIGHT_SHADOW_MAPS_SLOT = 10;



		const std::filesystem::path textureDirectory = L"engine/src/general/resources/textures";



		class TextureManager
		{
		public:
			static TextureManager& getInstance()
			{
				static TextureManager instance;
				return instance;
			}


			/// <param name="fileName">file name including directory</param>
			/// <param name="bindSlot">in which register in shader to bind</param>
			/// <param name="textureDesc">only need to set Usage, BindFlags, CPUAccessFlags and MiscFlags. Don't forget to add D3D11_RESOURCE_MISC_TEXTURECUBE to MiscFlags when adding cubemap texture </param>
			void addTexture2D(
				const std::wstring& fileName,
				UINT bindSlot,
				const D3D11_TEXTURE2D_DESC& textureDesc,
				size_t maxSize = 0,
				DirectX::DDS_LOADER_FLAGS loadFlags = DirectX::DDS_LOADER_DEFAULT,
				DirectX::DDS_ALPHA_MODE* alphaMode = nullptr)
			{
				assert(m_2dTextures.find(fileName) == m_2dTextures.end() && "Texture is already created! Error");

				auto texture = std::make_shared<Texture2D>();
				texture->createTextureFromFile(bindSlot, fileName, textureDesc, maxSize, loadFlags, alphaMode);
				m_2dTextures[fileName] = texture;


			}


			std::shared_ptr<Texture2D> getTexture2D(const std::wstring& fileName)
			{
				auto it = m_2dTextures.find(fileName);
				if (it == m_2dTextures.end())
				{
					return nullptr; // texture is not found
				}
				return it->second;
			}




		private:
			TextureManager() = default;
			TextureManager(const TextureManager&) = delete;
			TextureManager& operator=(const TextureManager&) = delete;

			std::unordered_map<std::wstring, std::shared_ptr<Texture2D>> m_2dTextures{};

		};

	}
}