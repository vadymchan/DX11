#pragma once
#include "../source/material/material.h"
#include "../source/math/ray.h"
#include "../source/math/intersection.h"
#include <memory>

namespace engine
{

	namespace scene
	{
		class SceneObject
		{
		public:
			virtual bool hit(const math::ray& r, const std::shared_ptr<math::Intersection>& near_t) = 0;
			virtual const Material& getMaterial() const = 0;


		};

	}
}