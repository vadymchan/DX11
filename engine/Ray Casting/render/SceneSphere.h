#pragma once
#include "SceneObject.h"
#include "../source/math/sphere.h"
#include "../source/material/coloredSphere.h"

namespace engine
{
	namespace scene
	{
		class SceneSphere : public SceneObject
		{
		public:
			SceneSphere(const math::Sphere& sphere, const Material& material)
			{
				this->sphere = std::make_unique<ColoredSphere>(ColoredSphere(material, sphere));
			}

			bool hit(const math::ray& r, const std::shared_ptr<math::Intersection>& intersection) override
			{
				return sphere->hit(r, *intersection.get());
			}

			const Material& getMaterial() const override
			{
				return sphere->material;
			}

		private:
			std::unique_ptr<ColoredSphere> sphere;
		};
	}
}

