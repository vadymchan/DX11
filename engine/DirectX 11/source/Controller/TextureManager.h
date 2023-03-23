#pragma once
#include <unordered_map>
#include "../Texture/Texture2D.h"
#include "../Texture/DDSTextureLoader11.h"
#include "../D3D/D3D.h"
#include "../Texture/SampleState.h"

namespace engine
{
	namespace DX
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

			void addSamplerState(
				const std::wstring& semanticName,
				UINT bindSlot,
				const D3D11_SAMPLER_DESC& desc)
			{
				assert(m_samplerStates.find(semanticName) == m_samplerStates.end() && "SamplerState is already created! Error");

				m_samplerStates[semanticName].initSampleState(bindSlot, desc);
			}

			SampleState& getSamplerState(const std::wstring& semanticName)
			{
				assert(m_samplerStates.find(semanticName) != m_samplerStates.end() && "SamplerState is not created! Error");

				return m_samplerStates[semanticName];


			}

			/// <param name="bindSlot">in which register in shader to bind</param>
			/// <param name="textureDesc">only need to set BindFlags, CPUAccessFlags and MiscFlags </param>
			void addTexture2D(
				const std::wstring& fileName,
				UINT bindSlot,
				const D3D11_TEXTURE2D_DESC& textureDesc,
				size_t maxSize = 0,
				DirectX::DDS_LOADER_FLAGS loadFlags = DirectX::DDS_LOADER_DEFAULT,
				DirectX::DDS_ALPHA_MODE* alphaMode = nullptr)
			{
				assert(m_2dTextures.find(fileName) == m_2dTextures.end() && "Texture is already created! Error");


				m_2dTextures[fileName].createTextureFromFile(bindSlot, textureDirectory + fileName, textureDesc, maxSize, loadFlags, alphaMode);


			}

			Texture2D& getTexture2D(const std::wstring& fileName)
			{
				assert(m_2dTextures.find(fileName) != m_2dTextures.end() && "Texture is not created! Error");

				return m_2dTextures[fileName];
			}


		private:
			TextureManager() = default;
			TextureManager(const TextureManager&) = delete;
			TextureManager& operator=(const TextureManager&) = delete;

			std::unordered_map<std::wstring, Texture2D> m_2dTextures{};
			std::unordered_map<std::wstring, SampleState> m_samplerStates{};

		};

	}
}