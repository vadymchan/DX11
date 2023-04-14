#include "SolidVector.h"

namespace engine::DX
{
    /*class Transform {
    public:
        float3 position;
        quat rotation;
        float3 scale;

        Transform(const float3& position = { 0, 0, 0 }, const quat& rotation = { 0, 0, 0, 1 }, const float3& scale = { 1, 1, 1 })
            : position(position), rotation(rotation), scale(scale) {}
    };*/

    class TransformSystem {
    public:
        using ID = SolidVector<float4x4>::ID;

        // Deleted copy constructor and assignment operator
        TransformSystem(const TransformSystem&) = delete;
        TransformSystem& operator=(const TransformSystem&) = delete;

        // Static method to access the single instance
        static TransformSystem& getInstance()
        {
            static TransformSystem instance;
            return instance;
        }

        // Add a new Transform
        ID addTransform(const float4x4& transform) {
            return transforms.insert(transform);
        }

        // Remove a Transform by ID
        void removeTransform(ID id) {
            transforms.erase(id);
        }

        // Get a Transform by ID (const and non-const versions)
        const float4x4& getTransform(ID id) const {
            return transforms[id];
        }

        float4x4& getTransform(ID id) {
            return transforms[id];
        }

        // Update a Transform by ID
        void updateTransform(ID id, const float4x4& newTransform) {
            transforms[id] = newTransform;
        }

        // Get the number of transforms
        size_t size() const {
            return transforms.size();
        }

    private:
        // Private constructor
        TransformSystem() {}

        SolidVector<float4x4> transforms;
    };
}
