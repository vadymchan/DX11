#pragma once

#include <algorithm>

#include "../D3D/D3D.h"
#include "../Controller/BufferManager.h"
#include "../Buffer/VertexBuffer.h"
#include "../../../general/utils/sort/RadixSort.h"
#include "../Buffer/IndexBuffer.h"
#include "../Texture/Texture2D.h"
#include "../Shader/VertexShader.h"
#include "../Shader/PixelShader.h"
#include "../Math/Colision/RaySphereIntersection.h"
#include "../Math/Transform/TransformSystem.h"
#include "../Utils/Camera.h"
#include "../Controller/TextureManager.h"

using namespace DirectX;

namespace engine::DX
{

	struct Particle
	{
		float4 color; // RGB tint + alpha
		float3 position;
		float rotationAngle;
		float rotationSpeed;
		float2 size;
		float3 constantSpeed;
		float lifetime;
	};

	class Emitter
	{
	public:

		// constructor for emitter without sphere render
		Emitter(const float3& initPosition, int spawnRate, int spawnRadius, const float4& particleColor, float boundingSphereRadius,
			float initialParticleSize = 0.1f, float initialRotationAngle = 0.0f,
			float verticalSpeed = 2.0f, int randomSpeedRange = 5, float randomSpeedScale = 1 / 2.5f,
			int rotationSpeedRange = 100, float rotationSpeedScale = 1 / 50.0f)
			: spawnRate(spawnRate)
			, spawnRadius(spawnRadius)
			, particleColor(particleColor)
			, initialParticleSize(initialParticleSize)
			, initialRotationAngle(initialRotationAngle)
			, verticalSpeed(verticalSpeed)
			, randomSpeedRange(randomSpeedRange)
			, randomSpeedScale(randomSpeedScale)
			, rotationSpeedRange(rotationSpeedRange)
			, rotationSpeedScale(rotationSpeedScale)
		{
			float4x4 sphereAroundEmitterMatrix = float4x4{
				{boundingSphereRadius, 0 , 0, initPosition.x},
				{0, boundingSphereRadius , 0, initPosition.y},
				{0, 0 , boundingSphereRadius, initPosition.z},
				{0, 0 , 0, 1},
			};
			transformID = TransformSystem::getInstance().addTransform(sphereAroundEmitterMatrix);

		}

		Emitter(TransformSystem::ID transformID, int spawnRate, int spawnRadius, const float4& particleColor,
			float initialParticleSize = 0.1f, float initialRotationAngle = 0.0f,
			float verticalSpeed = 2.0f, int randomSpeedRange = 5, float randomSpeedScale = 1 / 2.5f,
			int rotationSpeedRange = 100, float rotationSpeedScale = 1 / 50.0f)
			: transformID(transformID)
			, spawnRate(spawnRate)
			, spawnRadius(spawnRadius)
			, particleColor(particleColor)
			, initialParticleSize(initialParticleSize)
			, initialRotationAngle(initialRotationAngle)
			, verticalSpeed(verticalSpeed)
			, randomSpeedRange(randomSpeedRange)
			, randomSpeedScale(randomSpeedScale)
			, rotationSpeedRange(rotationSpeedRange)
			, rotationSpeedScale(rotationSpeedScale)
		{
			
		}

		void update(float dt);



		void emit(int numParticles);

		//setters
		void setVerticalSpeed(float speed) { verticalSpeed = speed; }
		void setRandomSpeedRange(int range) { randomSpeedRange = range; }
		void setRandomSpeedScale(float scale) { randomSpeedScale = scale; }
		void setRotationSpeedRange(int range) { rotationSpeedRange = range; }
		void setRotationSpeedScale(float scale) { rotationSpeedScale = scale; }

		//getters
		TransformSystem::ID getTransformID() const { return transformID; }
		float getVerticalSpeed() const { return verticalSpeed; }
		int getRandomSpeedRange() const { return randomSpeedRange; }
		float getRandomSpeedScale() const { return randomSpeedScale; }
		int getRotationSpeedRange() const { return rotationSpeedRange; }
		float getRotationSpeedScale() const { return rotationSpeedScale; }



		int spawnRate;
		int spawnRadius;
		float4 particleColor;

		std::vector<Particle> particles;

	protected:

		TransformSystem::ID transformID;

		const float initialParticleLifetime {1.f};
		//const float initialParticleLifetime {0.5f};
		const float initialParticleSize;
		const float initialRotationAngle;
		float verticalSpeed;
		int randomSpeedRange;
		float randomSpeedScale;
		int rotationSpeedRange;
		float rotationSpeedScale;

		float accumulatedTime{};
	};

	class ParticleSystem
	{
	public:
		enum class TextureType {
			RLU,
			DBF,
			MVEA,
			Count
		};

		void initParticleSystem() 
		{
			renderData.initBuffer(PARTICLE_DATA_INPUT_SLOT_2, { sizeof(GPUParticle) }, { 0 }, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
			renderIndices.initBuffer(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
			//resolutionConstantBuffer.setBufferData({ {} });
			resolutionConstantBuffer.initBuffer(SCREEN_RESOLUTION, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
			//resolutionConstantBuffer.createBuffer();
			depthCopyTexture.setBindSlot(DEPTH_BUFFER_COPY_TEXTURE_BIND_SLOT);
			setupDepthStencilState();
		}

		TransformSystem::ID addSmokeEmitter(const Emitter& smokeEmitter);

		bool findIntersection(const ray& r, TransformSystem::ID& instance, RayIntersection& intersection);

		void setParticleTextures(const std::array<std::weak_ptr<Texture2D>, (size_t)TextureType::Count>& particleTextures);

		void setShaders(std::weak_ptr<VertexShader> particleVertexShader, std::weak_ptr<PixelShader> particlePixelShader);


		void update(float dt);

		void render(Camera& camera);


	protected:
		struct GPUParticle
		{
			float3 position;
			float4 color;
			float2 uv;
			float lifetime;
			float rotation;
			float2 size;
		};


	private:

		void updateBuffers(Camera& camera);

		void updateDepthCopy()
		{
			// Get the depth stencil view
			ComPtr<ID3D11DepthStencilView> depthStencilView;
			g_devcon->OMGetRenderTargets(0, nullptr, &depthStencilView);

			// Get the underlying texture
			ComPtr<ID3D11Resource> resource;
			depthStencilView->GetResource(&resource);

			ComPtr<ID3D11Texture2D> depthTexture;
			if (SUCCEEDED(resource->QueryInterface(IID_PPV_ARGS(&depthTexture))))
			{

				// The descriptions don't match, perform the copy
				depthCopyTexture.copyTextureFromSource(depthTexture.Get());

			}
		}

		void updateResolutionConstantBuffer() 
		{
			auto& depthCopyTextureDesc = depthCopyTexture.getTextureDesc();

			if (resolutionConstantBufferData.width != depthCopyTextureDesc.Width 
				|| resolutionConstantBufferData.height != depthCopyTextureDesc.Height)
			{
					resolutionConstantBufferData.width = depthCopyTextureDesc.Width;
				resolutionConstantBufferData.height = depthCopyTextureDesc.Height;

				resolutionConstantBuffer.updateData({ resolutionConstantBufferData });
			}


		}

		void bindBuffers();

		void bindTextures();

		void bindShaders();

		void ParticleSystem::setupDepthStencilState()
		{
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
			ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

			// Depth test parameters
			depthStencilDesc.DepthEnable = true;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Disable depth writing
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

			// Stencil test parameters
			depthStencilDesc.StencilEnable = true;
			depthStencilDesc.StencilReadMask = 0xFF;
			depthStencilDesc.StencilWriteMask = 0xFF;

			// Stencil operations (if pixel is front-facing)
			depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			// Stencil operations (if pixel is back-facing)
			depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

				
			HRESULT hr = g_device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.ReleaseAndGetAddressOf());
			if (FAILED(hr))
			{
				PrintError(hr, L"Failed to create depth stencil state.\n");
			}

		}

		std::weak_ptr<VertexShader> vertexShader;
		std::weak_ptr<PixelShader> pixelShader;
		std::array<std::weak_ptr<Texture2D>, (size_t)TextureType::Count> particleTextures;

		ComPtr<ID3D11DepthStencilState> depthStencilState;

		Texture2D depthCopyTexture;


		struct ResolutionConstantBuffer
		{
			float width;
			float height;
			float padding1;
			float padding2;
		} resolutionConstantBufferData;
		ConstantBuffer<ResolutionConstantBuffer> resolutionConstantBuffer;

		std::vector<Emitter> smokeEmitters;
		VertexBuffer<GPUParticle> renderData;
		IndexBuffer renderIndices;
	};


}