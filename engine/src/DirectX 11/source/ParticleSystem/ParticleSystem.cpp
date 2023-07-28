#include "ParticleSystem.h"


namespace engine::DX
{

	void Emitter::update(float dt)
	{
		dt /= 10;
		for (auto& particle : particles)
		{
			//// update position
			//particle.position.x += particle.constantSpeed.x * dt;
			//particle.position.y += particle.constantSpeed.y * dt;
			//particle.position.z += particle.constantSpeed.z * dt;

			//// update size
			//particle.size.x += dt; // Increase size over time
			//particle.size.y += dt; // Increase size over time

			//// update rotation
			//particle.rotationAngle += particle.rotationSpeed * dt;

			// update alpha
			//if (particle.lifetime > 0.5f)
			//{
			//	particle.color.w = 1.0f - 2.0f * (particle.lifetime - 0.5f); // Alpha goes from 0 to 1
			//}
			//else
			//{
			//	particle.color.w = 2.0f * particle.lifetime; // Alpha goes from 1 to 0
			//}


			//// Decrease lifetime
			//particle.lifetime -= dt;

		}

		// Remove dead particles
		//particles.erase(std::remove_if(particles.begin(), particles.end(),
		//	[](const Particle& particle) { return particle.lifetime <= 0.0f; }), particles.end());

		accumulatedTime += dt;

		const int numParticles = static_cast<int>(spawnRate * accumulatedTime);
		if (numParticles > 0)
		{
			//emit(numParticles);
			accumulatedTime -= numParticles / static_cast<float>(spawnRate);
		}

	}

	void Emitter::emit(int numParticles)
	{

	/*	Particle particle;

		particle.color = particleColor;
		particle.

		particles.push_back(particle);*/

		const int rangeHalfInt = randomSpeedRange / 2;
		const float rangeHalfFloat = (float)randomSpeedRange / 2;
		const int rotationRangeHalf = rotationSpeedRange / 2;

		for (int i = 0; i < numParticles; ++i)
		{
			Particle newParticle;
			newParticle.color = particleColor;
			float offset_x = spawnRadius == 0 ? 0 : (rand() % spawnRadius) - spawnRadius;
			float offset_y = spawnRadius == 0 ? 0 : (rand() % spawnRadius) - spawnRadius;
			float offset_z = spawnRadius == 0 ? 0 : (rand() % spawnRadius) - spawnRadius;
			const auto& emitterTransformMatrix = TransformSystem::getInstance().getTransform(transformID).Transpose();
			float3 emitterPosition = emitterTransformMatrix.Translation();
			newParticle.position = emitterPosition + float3(offset_x, offset_y, offset_z);
			newParticle.rotationAngle = initialRotationAngle;
			newParticle.size = float2(initialParticleSize, initialParticleSize);
			newParticle.rotationSpeed = (float)(rand() % rotationSpeedRange - rotationRangeHalf) * rotationSpeedScale;
			newParticle.constantSpeed = float3( // Initialize with random horizontal and constant vertical motion
				(rand() % randomSpeedRange - rangeHalfFloat) * randomSpeedScale,
				verticalSpeed,
				(rand() % randomSpeedRange - rangeHalfFloat) * randomSpeedScale);

			newParticle.lifetime = initialParticleLifetime;

			particles.push_back(newParticle);
		}
	}

	TransformSystem::ID ParticleSystem::addSmokeEmitter(const Emitter& smokeEmitter)
	{
		Emitter& newEmitter = smokeEmitters.emplace_back(smokeEmitter);

		return newEmitter.getTransformID();
	}

	bool ParticleSystem::findIntersection(const ray& r, TransformSystem::ID& instance, RayIntersection& intersection)
	{
		for (auto& emitter : smokeEmitters)
		{

			auto transformMatrix = TransformSystem::getInstance().getTransform(emitter.getTransformID()).Transpose();
			float3 boundingSpherePosition = transformMatrix.Translation();
			float boundingSphereRadius = transformMatrix._11;
			if (raySphereIntersect(r, Sphere{ boundingSpherePosition, boundingSphereRadius }, intersection))
			{
				instance = emitter.getTransformID();
				return true;
			}
		}

		return false;
	}

	void ParticleSystem::setParticleTextures(const std::array<std::weak_ptr<Texture2D>, (size_t)TextureType::Count>& particleTextures)
	{
		this->particleTextures = particleTextures;
	}

	void ParticleSystem::setShaders(std::weak_ptr<VertexShader> particleVertexShader, std::weak_ptr<PixelShader> particlePixelShader)
	{
		vertexShader = particleVertexShader;
		pixelShader = particlePixelShader;
	}

	void ParticleSystem::update(float dt)
	{
		for (auto& emitter : smokeEmitters)
		{
			emitter.update(dt);
		}
	}

	void ParticleSystem::render(Camera& camera)
	{
		bindShaders();

		updateBuffers(camera);
		bindBuffers();

		bindTextures();

		g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		g_devcon->DrawIndexed(renderIndices.getBufferData().size(), 0, 0);

		UnbindVertexBuffer(PARTICLE_DATA_INPUT_SLOT_2);
	}

	void ParticleSystem::updateBuffers(Camera& camera)
	{
		float4x4 viewMatrix = camera.getViewMatrix();

		size_t totalParticles = 0;
		for (const auto& emitter : smokeEmitters)
		{
			totalParticles += emitter.particles.size();
		}

		std::vector<GPUParticle> gpuParticles;
		gpuParticles.reserve(totalParticles);

		for (const auto& emitter : smokeEmitters)
		{
			for (const auto& particle : emitter.particles)
			{
				GPUParticle gpuParticle;

				// Convert particle position to view space
				float4 particleTransform = float4::Transform(float4(particle.position.x, particle.position.y, particle.position.z, 1), viewMatrix);
				gpuParticle.position = float3(particleTransform.x, particleTransform.y, particleTransform.z);
				gpuParticle.rotation = particle.rotationAngle;
				gpuParticle.size = particle.size;
				gpuParticle.color = particle.color;
				gpuParticle.lifetime = particle.lifetime;


				gpuParticles.push_back(gpuParticle);
			}
		}


		std::sort(gpuParticles.begin(), gpuParticles.end(), [](const GPUParticle& a, const GPUParticle& b) {
			return a.position.z > b.position.z; });



		//update vertex buffer

		constexpr int VERTEX_PER_QUAD = 4;
		std::vector<GPUParticle> bufferData;
		bufferData.reserve(VERTEX_PER_QUAD * gpuParticles.size());

		for (auto& particle : gpuParticles)
		{
			std::fill_n(std::back_inserter(bufferData), VERTEX_PER_QUAD, particle);
		}

		renderData.updateBuffer(bufferData);


		// update index buffer
		constexpr int INDICES_PER_QUAD = 6;
		std::vector<uint32_t> indexBufferData;
		indexBufferData.reserve(INDICES_PER_QUAD * gpuParticles.size());

		for (uint32_t i = 0; i < static_cast<uint32_t>(gpuParticles.size()); ++i)
		{
			uint32_t currentIndex = i * VERTEX_PER_QUAD;
			const uint32_t firstVertex = currentIndex;
			const uint32_t secondVertex = currentIndex + 1;
			const uint32_t thirdVertex = currentIndex + 2;
			const uint32_t fourthVertex = currentIndex + 3;
			indexBufferData.insert(indexBufferData.end(),
				{ firstVertex, secondVertex, thirdVertex, thirdVertex, fourthVertex, firstVertex }
			);
		}



		renderIndices.setBufferData(indexBufferData);

	}

	void ParticleSystem::bindBuffers()
	{
		renderData.setBuffer();
		renderIndices.setBuffer();
	}

	void ParticleSystem::bindTextures()
	{
		for (auto& texture : particleTextures)
		{
			if (auto textureLocked = texture.lock())
			{
				textureLocked->bind();
			}
			else
			{
				std::cerr << "Failed to bind texture in ParticleSystem: weak_ptr<Texture2D> is either expired or not set.\n";
			}
		}


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

		depthCopyTexture.bind();


	}

	void ParticleSystem::bindShaders()
	{
		//bind vertex shader
		if (auto vertexShaderLocked = vertexShader.lock())
		{
			vertexShaderLocked->bind();
		}
		else
		{
			std::cerr << "Failed to bind vertex shader in ParticleSystem: weak_ptr<VertexShader> is either expired or not set.\n";
		}

		//bind pixel shader
		if (auto pixelShaderLocked = pixelShader.lock())
		{
			pixelShaderLocked->bind();
		}
		else
		{
			std::cerr << "Failed to bind pixel shader in ParticleSystem: weak_ptr<PixelShader> is either expired or not set.\n";
		}
	}

}