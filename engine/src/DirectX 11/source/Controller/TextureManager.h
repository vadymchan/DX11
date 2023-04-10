#pragma once
#include <unordered_map>
#include "../Texture/Texture2D.h"
#include "../Texture/DDSTextureLoader11.h"
#include "../D3D/D3D.h"
#include <filesystem>

namespace engine
{
	namespace DX
	{
#define COLOR_TEXTURE_BIND_SLOT 0

		


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
			/// <param name="textureDesc">only need to set BindFlags, CPUAccessFlags and MiscFlags. Don't forget to add D3D11_RESOURCE_MISC_TEXTURECUBE to MiscFlags when adding cubemap texture </param>
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

			const std::shared_ptr<Texture2D>& getTexture2D(const std::wstring& fileName)
			{
				assert(m_2dTextures.find(fileName) != m_2dTextures.end() && "Texture is not created! Error");

				return m_2dTextures[fileName];
			}


		private:
			TextureManager() = default;
			TextureManager(const TextureManager&) = delete;
			TextureManager& operator=(const TextureManager&) = delete;

			std::unordered_map<std::wstring, std::shared_ptr<Texture2D>> m_2dTextures{};

		};

	}
}