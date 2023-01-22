#include "meshRC.h"

bool RC::engine::Mesh::hit(ray r, float& near, glm::vec3& intersectionN)
{
    bool hitAny{};

	//change ray to model space  (origin + vector)
	r *= glm::inverse(tMat);

	for (auto& triangle : *(triangles.get()))
	{
		if (triangle.hit(r, near))
		{
			intersectionN = triangle.getN();
			hitAny = true;
		}
	}

    return hitAny;
}
