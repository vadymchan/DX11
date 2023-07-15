#include "LightSystem.h"

namespace engine::DX
{


	void LightSystem::setPerViewDirectionalLight(int lightIndex)
	{


		PerView2dShadowMap constantBuffer
		{
			m_lightConstantBuffer.g_directionalLights[lightIndex].shadowViewMatrix,
			m_lightConstantBuffer.g_directionalLights[lightIndex].shadowProjMatrix,
		};

		g_devcon->OMSetRenderTargets(0, nullptr, directionalLightViews[lightIndex].Get());
		//g_devcon->ClearDepthStencilView(directionalLightViews[lightIndex].Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);


		bind2DShadowMap(constantBuffer);
	}

	void LightSystem::setPerViewSpotLight(int lightIndex)
	{

		PerView2dShadowMap constantBuffer
		{
			m_lightConstantBuffer.g_spotLights[lightIndex].shadowViewMatrix,
			m_lightConstantBuffer.g_spotLights[lightIndex].shadowProjMatrix,
		};

		g_devcon->OMSetRenderTargets(0, nullptr, spotLightViews[lightIndex].Get());
		//g_devcon->ClearDepthStencilView(spotLightViews[lightIndex].Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);


		bind2DShadowMap(constantBuffer);
	}

	void LightSystem::setPerViewFlashLight(int lightIndex)
	{

		PerView2dShadowMap constantBuffer
		{
			m_lightConstantBuffer.g_flashLights[lightIndex].shadowViewMatrix,
			m_lightConstantBuffer.g_flashLights[lightIndex].shadowProjMatrix,
		};

		g_devcon->OMSetRenderTargets(0, nullptr, flashLightViews[lightIndex].Get());
		//g_devcon->ClearDepthStencilView(flashLightViews[lightIndex].Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);


		bind2DShadowMap(constantBuffer);
	}

	void LightSystem::setPerViewPointLight(int lightIndex)
	{
		PerView3dShadowMap constantBuffer
		{
			m_lightConstantBuffer.g_pointLights[lightIndex].shadowViewMatrices,
			m_lightConstantBuffer.g_pointLights[lightIndex].shadowProjMatrix,
		};

		g_devcon->OMSetRenderTargets(0, nullptr, pointLightViews[lightIndex].Get());
		//g_devcon->ClearDepthStencilView(pointLightViews[lightIndex].Get(), D3D11_CLEAR_DEPTH, 0.0f, 0);


		perView3dConstantBuffer.updateData({ constantBuffer });
		perView3dConstantBuffer.setGeometryShaderBuffer();
	}
	LightSystem::PointLightID LightSystem::addPointLight(const PointLight& pointLight)
	{
		return pointLights.insert(pointLight);
	}

	LightSystem::DirectionalLightID LightSystem::addDirectionalLight(const DirectionalLight& directionalLight)
	{
		return directionalLights.insert(directionalLight);
	}

	LightSystem::SpotLightID LightSystem::addSpotLight(const SpotLight& spotLight)
	{
		return spotLights.insert(spotLight);
	}

	/// <param name="outerAngle"> pass cosine of degrees in radians (cos(radians(degrees)))</param>
	/// <param name="textureMask"> pass cosine of degrees in radians (cos(radians(degrees)))</param>
	LightSystem::FlashLightID LightSystem::addFlashLight(
		const float3& color,
		float intensity,
		float innerAngle,
		float outerAngle,
		float radius,
		const Attenuation& attenuation,
		const std::wstring& textureMask)
	{
		
		D3D11_TEXTURE2D_DESC desc{};
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		TextureManager::getInstance().addTexture2D(textureMask, FLASHLIGHT_TEXTURE_BIND_SLOT, desc);

		float3 position;
		float3 direction;
		bool attachedToCamera = false;
		if (camera != nullptr)
		{
			position = camera->position();
			direction = camera->forward();
			attachedToCamera = true;
		}

		const std::shared_ptr<Texture2D>& texture = TextureManager::getInstance().getTexture2D(textureMask);

		FlashLight flashLight =
		{
			{ color, intensity },
			innerAngle, outerAngle,
			radius,
			attenuation,
			attachedToCamera,
			position,
			direction,
			texture
		};

		return flashLights.insert(flashLight);
	}

	void LightSystem::updateTextureMasks()
	{
		for (size_t i = 0; i < flashLights.size(); ++i)
		{
			flashLights[i].textureMask->bind();
		}
	}

	void LightSystem::updateFlashLightConstantBuffer(FlashLight& src, LightConstantBuffer::FlashLight& dst)
	{
		if (src.attachedToCamera)
		{
			src.currentPosition = camera->position();
			src.currentDirection = camera->forward();
		}

		dst.position = src.currentPosition;
		dst.direction = src.currentDirection;

		//dst.position = float3::Transform(src.currentPosition, camera->getViewMatrix());
		//dst.direction = float3::TransformNormal(src.currentDirection, camera->getViewMatrix());
		dst.innerAngle = src.innerAngle;
		dst.outerAngle = src.outerAngle;
		dst.radius = src.radius;
		dst.attenuation.constantAttenuation = src.attenuation.constantAttenuation;
		dst.attenuation.linearAttenuation = src.attenuation.linearAttenuation;
		dst.attenuation.quadraticAttenuation = src.attenuation.quadraticAttenuation;
		dst.base.color = src.base.color;
		dst.base.intensity = src.base.intensity;

		computeBeamLightMatrices(dst.outerAngle, src.currentPosition, src.currentDirection, 0.1, 100, dst.shadowViewMatrix, dst.shadowProjMatrix);
	}

	void LightSystem::updateSpotLightConstantBuffer(const SpotLight& src, LightConstantBuffer::SpotLight& dst)
	{

		dst.base.color = src.base.color;
		dst.base.intensity = src.base.intensity;
		dst.innerAngle = src.innerAngle;
		dst.outerAngle = src.outerAngle;
		dst.radius = src.radius;
		dst.attenuation.constantAttenuation = src.attenuation.constantAttenuation;
		dst.attenuation.linearAttenuation = src.attenuation.linearAttenuation;
		dst.attenuation.quadraticAttenuation = src.attenuation.quadraticAttenuation;
		const float4x4& worldMatrix = TransformSystem::getInstance().getTransform(src.transformID);
		const float3 positionWorld = worldMatrix.Transpose().Translation();
		dst.position = float3::Transform(positionWorld, camera->getViewMatrix());
		//dst.direction = src.direction;
		dst.direction = float3::TransformNormal(src.direction, camera->getViewMatrix());

		computeBeamLightMatrices(dst.outerAngle, positionWorld, src.direction, 0.1, 100, dst.shadowViewMatrix, dst.shadowProjMatrix);

	}

	void LightSystem::updateDirectionalLightConstantBuffer(const DirectionalLight& src, LightConstantBuffer::DirectionalLight& dst)
	{
		dst.base.color = src.base.color;
		dst.base.intensity = src.base.intensity;
		dst.solidAngle = src.solidAngle;
		//dst.direction = src.direction; // for world space
		dst.direction = float3::TransformNormal(src.direction, camera->getViewMatrix());

		computeLightSpaceFrustumDirectionalLight(src.direction, dst.shadowViewMatrix, dst.shadowProjMatrix);
	}

	void LightSystem::updatePointLightConstantBuffer(const PointLight& src, LightConstantBuffer::PointLight& dst)
	{
		dst.base.color = src.base.color;
		dst.base.intensity = src.base.intensity;
		dst.radius = src.radius;
		dst.attenuation.constantAttenuation = src.attenuation.constantAttenuation;
		dst.attenuation.linearAttenuation = src.attenuation.linearAttenuation;
		dst.attenuation.quadraticAttenuation = src.attenuation.quadraticAttenuation;
		const float4x4& worldMatrix = TransformSystem::getInstance().getTransform(src.transformID);
		const float3& srcPosition = worldMatrix.Transpose().Translation();
		dst.position = srcPosition; // for world space
		//dst.position = float3::Transform(srcPosition, camera->getViewMatrix());

		computePointLightMatrices(srcPosition, 0.1, 100, dst.shadowViewMatrices, dst.shadowProjMatrix);
	}

	void LightSystem::updateBuffer()
	{

		if (camera != nullptr)
		{
			m_buffer.createBuffer(); // creates buffer if it's not created yet

			

			//add camera position
			//--------------------------------------------------------
			const float3& cameraPos = camera->position();
			m_lightConstantBuffer.g_cameraPosition = float4(cameraPos.x, cameraPos.y, cameraPos.z, 1);
			//--------------------------------------------------------

			//add flashlight
			//--------------------------------------------------------
			m_lightConstantBuffer.g_numFlashLights = flashLights.size();
			for (size_t i = 0; i < flashLights.size(); i++)
			{
				updateFlashLightConstantBuffer(flashLights[i], m_lightConstantBuffer.g_flashLights[i]);
			}
			//--------------------------------------------------------

			//directional lights
			//--------------------------------------------------------
			m_lightConstantBuffer.g_numDirectionalLights = directionalLights.size();
			for (size_t i = 0; i < directionalLights.size(); i++)
			{
				updateDirectionalLightConstantBuffer(directionalLights[i], m_lightConstantBuffer.g_directionalLights[i]);
			}
			//--------------------------------------------------------

			//pointLights
			//--------------------------------------------------------
			m_lightConstantBuffer.g_numPointLights = pointLights.size();
			for (size_t i = 0; i < pointLights.size(); i++)
			{
				updatePointLightConstantBuffer(pointLights[i], m_lightConstantBuffer.g_pointLights[i]);
			}
			//--------------------------------------------------------

			//spotlights
			//--------------------------------------------------------
			m_lightConstantBuffer.g_numSpotLights = spotLights.size();
			for (size_t i = 0; i < spotLights.size(); i++)
			{
				updateSpotLightConstantBuffer(spotLights[i], m_lightConstantBuffer.g_spotLights[i]);
			}
			//--------------------------------------------------------

			m_buffer.updateData({ m_lightConstantBuffer });

		}
	}

	void LightSystem::initTexture2DShadowMap(UINT arraySize, UINT bindSlot, Texture2D& shadowMaps, std::vector<ComPtr<ID3D11DepthStencilView>>& depthStencilViews)
	{
		// Create a depth stencil texture
		D3D11_TEXTURE2D_DESC descDepth;
		ZeroMemory(&descDepth, sizeof(descDepth));
		descDepth.Width = SHADOW_MAP_SIZE;
		descDepth.Height = SHADOW_MAP_SIZE;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = arraySize;
		descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;

		// Create the shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.MipLevels = descDepth.MipLevels;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.ArraySize = descDepth.ArraySize;


		shadowMaps.createTextureFromMemory(descDepth, nullptr, srvDesc, bindSlot);

		// Create a depth stencil view for each texture in the array
		for (UINT i = 0; i < arraySize; ++i)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
			ZeroMemory(&descDSV, sizeof(descDSV));
			descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			descDSV.Texture2DArray.MipSlice = 0;
			descDSV.Texture2DArray.FirstArraySlice = i;
			descDSV.Texture2DArray.ArraySize = 1;

			ComPtr<ID3D11DepthStencilView> depthStencilView;
			HRESULT result = g_device->CreateDepthStencilView(shadowMaps.getTexture2DView(), &descDSV, depthStencilView.GetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Depth Stencil View was not created!\n");
			}

			depthStencilViews.push_back(depthStencilView);
		}

	}


	// general function for computing the view and perspective matrices for both spot and flash lights

	void LightSystem::initCubeShadowMap(UINT arraySize, UINT bindSlot, Texture2D& shadowMaps, std::vector<ComPtr<ID3D11DepthStencilView>>& depthStencilViews)
	{
		constexpr UINT CUBEMAP_FACES = 6;

		// Create a depth stencil texture
		D3D11_TEXTURE2D_DESC descDepth;
		ZeroMemory(&descDepth, sizeof(descDepth));
		descDepth.Width = SHADOW_MAP_SIZE;
		descDepth.Height = SHADOW_MAP_SIZE;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = CUBEMAP_FACES * arraySize;
		descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		// Create the shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = 1;
		srvDesc.TextureCubeArray.NumCubes = arraySize;


		shadowMaps.createTextureFromMemory(descDepth, nullptr, srvDesc, bindSlot);

		// Create a depth stencil view for each cube map
		for (UINT i = 0; i < arraySize; ++i)
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
			ZeroMemory(&descDSV, sizeof(descDSV));
			descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			descDSV.Texture2DArray.MipSlice = 0;
			descDSV.Texture2DArray.FirstArraySlice = i * CUBEMAP_FACES;
			descDSV.Texture2DArray.ArraySize = CUBEMAP_FACES;

			ComPtr<ID3D11DepthStencilView> depthStencilView;
			HRESULT result = g_device->CreateDepthStencilView(shadowMaps.getTexture2DView(), &descDSV, depthStencilView.GetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Depth Stencil View was not created!\n");
			}

			depthStencilViews.push_back(depthStencilView);
		}
	}

	void LightSystem::computeLightSpaceFrustumDirectionalLight(const float3& lightDir, float4x4& lightViewMatrix, float4x4& lightProjMatrix)
	{

		// Invert the view-projection matrix to get the frustum corners in world space
		float4x4 invViewProjMatrix = camera->getIPV();

		// Define the frustum corners in homogeneous clip space
		float4 frustumCorners[8] = {
			float4(-1.0f, -1.0f, 0.0f, 1.0f), // near bottom left
			float4(1.0f, -1.0f, 0.0f, 1.0f),  // near bottom right
			float4(-1.0f, 1.0f, 0.0f, 1.0f),  // near top left
			float4(1.0f, 1.0f, 0.0f, 1.0f),   // near top right
			float4(-1.0f, -1.0f, 1.0f, 1.0f), // far bottom left
			float4(1.0f, -1.0f, 1.0f, 1.0f),  // far bottom right
			float4(-1.0f, 1.0f, 1.0f, 1.0f),  // far top left
			float4(1.0f, 1.0f, 1.0f, 1.0f)    // far top right
		};

		// Transform the frustum corners to world space
		for (int i = 0; i < 8; ++i) {
			frustumCorners[i] = float4::Transform(frustumCorners[i], invViewProjMatrix);
			frustumCorners[i] /= frustumCorners[i].w;
		}


		// Compute the centroid of the frustum corners
		float3 frustumCenter = float3::Zero;
		for (int i = 0; i < 8; ++i) {
			const float3 frustrumCorner = float3(frustumCorners[i].x, frustumCorners[i].y, frustumCorners[i].z);
			frustumCenter += frustrumCorner / 8.0f;
		}

		// Create the light view matrix
		float3 upDir = abs(lightDir.y) < 0.999f ? float3::Up : float3::Forward;
		lightViewMatrix = DirectX::XMMatrixLookAtLH(frustumCenter, frustumCenter + lightDir, upDir);

		// Transform the frustum corners to light space
		for (int i = 0; i < 8; ++i) {
			frustumCorners[i] = float4::Transform(frustumCorners[i], lightViewMatrix);
		}


		// Use the center and diameter to determine the X and Y ranges
		float3 mins = float3(std::numeric_limits<float>::max());
		float3 maxs = float3(std::numeric_limits<float>::lowest());


		//-------------------------------------

		// Find the minimum and maximum Z values
		for (int i = 0; i < 8; ++i) {
			mins.x = std::min(mins.x, frustumCorners[i].x);
			maxs.x = std::max(maxs.x, frustumCorners[i].x);
			mins.y = std::min(mins.y, frustumCorners[i].y);
			maxs.y = std::max(maxs.y, frustumCorners[i].y);

			mins.z = std::min(mins.z, frustumCorners[i].z);
			maxs.z = std::max(maxs.z, frustumCorners[i].z);
		}


		//remap X and Y coordinates (for square like box)
		float diameter = std::max(maxs.x - mins.x, maxs.y - mins.y);

		maxs.x = mins.x + diameter;
		maxs.y = mins.y + diameter;

		// shadow flickering 
		//-------------------------------------


		float texelSize = diameter / SHADOW_MAP_SIZE;

		auto test3 = mins.x / texelSize;
		auto test4 = mins.y / texelSize;

		auto test = floor(mins.x / texelSize);
		auto test1 = floor(mins.y / texelSize);

		float3 roundedMins = float3(
			floor(mins.x / texelSize) * texelSize,
			floor(mins.y / texelSize) * texelSize,
			floor(mins.z / texelSize) * texelSize
		);

		float3 translation = mins - roundedMins;

		mins -= translation;
		maxs -= translation;

		//-------------------------------------


		float margin = 5; // adjust for scene
		mins.z -= margin;


		lightProjMatrix = float4x4(DirectX::XMMatrixOrthographicOffCenterLH(
			mins.x, // left
			maxs.x, // right
			mins.y, // bottom
			maxs.y, // top
			maxs.z,  // far
			mins.z // near
		));


		lightViewMatrix = lightViewMatrix.Transpose();
		lightProjMatrix = lightProjMatrix.Transpose();
	}

	void LightSystem::computeBeamLightMatrices(float fov, const float3& lightPos, const float3& lightDir, float nearPlane, float farPlane, float4x4& lightViewMatrix, float4x4& lightProjMatrix)
	{
		float3 up = abs(lightDir.y) < 0.99f ? float3::Up : float3::Forward;
		lightViewMatrix = DirectX::XMMatrixLookAtLH(lightPos, lightPos + lightDir, up);
		lightProjMatrix = float4x4(DirectX::XMMatrixPerspectiveFovLH(fov * 2, 1, nearPlane, farPlane)) * Camera::reverseDepthMatrix;

		lightViewMatrix = lightViewMatrix.Transpose();
		lightProjMatrix = lightProjMatrix.Transpose();
	}

	void LightSystem::computePointLightMatrices(const float3& lightPos, float nearPlane, float farPlane, std::array<float4x4, 6>& lightViewMatrices, float4x4& lightProjMatrix)
	{

		// Compute the view matrices for each face of the cube map
		lightViewMatrices = {
			DirectX::XMMatrixLookAtLH(lightPos, lightPos + float3::Right, float3::Up),
			DirectX::XMMatrixLookAtLH(lightPos, lightPos + float3::Left, float3::Up),
			DirectX::XMMatrixLookAtLH(lightPos, lightPos + float3::Up, float3::Forward),
			DirectX::XMMatrixLookAtLH(lightPos, lightPos + float3::Down, float3::Backward),
			DirectX::XMMatrixLookAtLH(lightPos, lightPos + float3::Backward, float3::Up),
			DirectX::XMMatrixLookAtLH(lightPos, lightPos + float3::Forward, float3::Up)
		};

		// Compute the projection matrix
		lightProjMatrix = float4x4(DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 1.0f, nearPlane, farPlane)) * Camera::reverseDepthMatrix;

		for (size_t i = 0; i < 6; i++)
		{
			lightViewMatrices[i] = lightViewMatrices[i].Transpose();
		}
		lightProjMatrix = lightProjMatrix.Transpose();

	}

}