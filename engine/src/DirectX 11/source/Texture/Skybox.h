#pragma once

#include "../D3D/D3D.h"
#include "../Controller/TextureManager.h"
#include "Texture2D.h"
#include <array>
#include <vector>
#include "../Controller/ShaderManager.h"
#include "../Utils/Camera.h"

namespace engine::DX
{
	class Skybox
	{
	public:
		void initSkybox(const std::wstring& textureFileName);

		void render(Camera& camera);

		void setTexture(const std::wstring& textureFileName)
		{
			m_textureFileName = textureFileName;
		}

		const std::wstring& getTexture() const
		{
			return m_textureFileName;
		}

	private:
		std::wstring m_textureFileName;
		const static std::wstring VERTEX_SHADER_PATH;
		const static std::wstring PIXEL_SHADER_PATH;
	};




}