#pragma once
#include "SolidVector.h"


namespace engine::DX
{


	class Transform {
	public:
		float3 position;
		quat rotation;
		float3 scale;

		Transform(const float3& position = { 0, 0, 0 }, const quat& rotation = { 0, 0, 0, 1 }, const float3& scale = { 1, 1, 1 })
			: position(position), rotation(rotation), scale(scale) {}
	};

	class TransformSystem {
	public:
		using ID = SolidVector<Transform>::ID;

		TransformSystem() {}

		// Add a new Transform
		ID addTransform(const Transform& transform) {
			return transforms.insert(transform);
		}

		// Remove a Transform by ID
		void removeTransform(ID id) {
			transforms.erase(id);
		}

		// Get a Transform by ID (const and non-const versions)
		const Transform& getTransform(ID id) const {
			return transforms[id];
		}

		Transform& getTransform(ID id) {
			return transforms[id];
		}

		// Update a Transform by ID
		void updateTransform(ID id, const Transform& newTransform) {
			transforms[id] = newTransform;
		}

		// Get the number of transforms
		size_t size() const {
			return transforms.size();
		}

	private:
		SolidVector<Transform> transforms;
	};

}
