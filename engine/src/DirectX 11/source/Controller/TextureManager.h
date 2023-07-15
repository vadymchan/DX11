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
		constexpr UINT COLOR_TEXTURE_BIND_SLOT				= 0;
		constexpr UINT ALBEDO_TEXTURE_BIND_SLOT				= 0;
		constexpr UINT ROUGHNESS_TEXTURE_BIND_SLOT			= 1;
		constexpr UINT METALNESS_TEXTURE_BIND_SLOT			= 2;
		constexpr UINT NORMAL_TEXTURE_BIND_SLOT				= 3;
		constexpr UINT IRRADIANCE_MAP_BIND_SLOT				= 4;
		constexpr UINT PREFILTERED_MAP_BIND_SLOT			= 5;
		constexpr UINT BRDF_LUT_BIND_SLOT					= 6;
		constexpr UINT FLASHLIGHT_TEXTURE_BIND_SLOT			= 7;
		constexpr UINT DIRECTIONAL_LIGHT_SHADOW_MAPS_SLOT	= 8;
		constexpr UINT SPOT_LIGHT_SHADOW_MAPS_SLOT			= 9;
		constexpr UINT POINT_LIGHT_SHADOW_MAPS_SLOT			= 10;
		constexpr UINT FLASH_LIGHT_SHADOW_MAPS_SLOT			= 11;
		constexpr UINT NOISE_TEXTURE_BIND_SLOT				= 12;

		constexpr UINT SMOKE_RLU_TEXTURE_BIND_SLOT			= 0;
		constexpr UINT SMOKE_DBF_TEXTURE_BIND_SLOT			= 1;
		constexpr UINT SMOKE_MVEA_TEXTURE_BIND_SLOT			= 2;
		constexpr UINT DEPTH_BUFFER_COPY_TEXTURE_BIND_SLOT	= 3;

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
				texture->createTextureFromFile(fileName, textureDesc, bindSlot, maxSize, loadFlags, alphaMode);
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