#pragma once


namespace Engine
{


	struct Material
	{
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;

		Material()
		{
			ambient = glm::vec3(0);
			diffuse = glm::vec3(0);
			specular = glm::vec3(0);
			shininess = 0;
		}

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

