#pragma once

#include "TransformSystem.h"
#include "SolidVector.h"
#include "D3D/D3D.h"

namespace engine::DX
{
    class LightSystem {
    public:
       

        struct BaseLight {
            TransformSystem::ID transformID;
            float3 color;
            float intensity;
        };

        

        struct PointLight : public BaseLight {
            float range;
        };

        struct DirectionalLight : public BaseLight {
            // Direction will be derived from the Transform
        };

        struct SpotLight : public PointLight {
            float innerAngle;
            float outerAngle;
        };

        using PointLightID = SolidVector<PointLight>::ID;


        // Constructor
        LightSystem(TransformSystem& transformSystem) : transformSystem(transformSystem) {}

        // Add a PointLight
        PointLightID addPointLight(const Transform& transform, const float3& color, float intensity, float range)
        {
            TransformSystem::ID transformID = transformSystem.addTransform(transform);
            PointLight light{ transformID, color, intensity, range };
            return pointLights.insert(light);
        }

        // Other add methods for DirectionalLight and SpotLight go here
        // ...

    protected:

        // Add, remove, and update light methods go here
        // Store lights in separate containers for each type
        SolidVector<PointLight> pointLights;
        SolidVector<DirectionalLight> directionalLights;
        SolidVector<SpotLight> spotLights;


        TransformSystem& transformSystem;

       
    };

}

