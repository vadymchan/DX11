#pragma once
#include "../../Utils/SolidVector.h"
#include "../../D3D/D3D.h"

namespace engine::DX
{


    class TransformSystem {
    public:
        using ID = SolidVector<float4x4>::ID;
       
        TransformSystem(const TransformSystem&) = delete;
        TransformSystem& operator=(const TransformSystem&) = delete;
     
        static TransformSystem& getInstance()
        {
            static TransformSystem instance;
            return instance;
        }
      
        ID addTransform(const float4x4& transform) {
            return transforms.insert(transform);
        }

        void removeTransform(ID id) {
            transforms.erase(id);
        }

        const float4x4& getTransform(ID id) const {
            return transforms[id];
        }

        float4x4& getTransform(ID id) {
            return transforms[id];
        }

        void updateTransform(ID id, const float4x4& newTransform) {
            transforms[id] = newTransform;
        }

        size_t size() const {
            return transforms.size();
        }

    private:
        TransformSystem() {}

        SolidVector<float4x4> transforms;
    };
}
