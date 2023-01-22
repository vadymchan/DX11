#pragma once


namespace RC::engine
{

	struct Material
	{
		glm::vec3 ambient{glm::vec3(1)};
		glm::vec3 diffuse{};
		glm::vec3 specular{};
		float shininess{};

		Material() = default;

		Material(const glm::vec3& ambient,
			const glm::vec3& diffuse,
			const glm::vec3& specular,
			float shininess)
			: ambient(ambient)
			, diffuse(diffuse)
			, specular(specular)
			, shininess(shininess)
		{}

	};


}

