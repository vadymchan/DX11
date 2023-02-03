#pragma once
#include "SceneObject.h"
#include "../source/math/plane.h"
#include "../source/material/coloredPlane.h"


namespace engine
{
	namespace scene
	{
		class ScenePlane : public SceneObject
		{
		public:
			ScenePlane(const math::Plane& plane, const Material& material)
			{
				this->plane = std::make_unique<ColoredPlane>(ColoredPlane(material, plane));
			}

			bool hit(const math::ray& r, const std::shared_ptr<math::Intersection>& intersection) override
			{
				return plane->hit(r, *intersection.get());
			}

			const Material& getMaterial() const override
			{
				return plane->material;
			}
		private:
			std::unique_ptr<ColoredPlane> plane;
		};
	}
}

