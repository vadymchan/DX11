#pragma once
#include "../source/math/directionalLight.h"
#include "../source/material/coloredSphere.h"
#include "../source/material/coloredDirectionLight.h"
#include "../source/material/coloredPointLight.h"
#include "../source/material/coloredSpotLight.h"
#include "../source/material/coloredTriangle.h"
#include "../source/material/coloredPlane.h"
#include "../source/material/coloredMesh.h"
#include "../source/math/intersection.h"
#include "../utils/Dragger/IObjectMover.h"
#include "../utils/Dragger/SphereMover.h"
#include "../utils/Dragger/MeshMover.h"
#include "../utils/Dragger/PlaneMover.h"
#include "../utils/Dragger/TriangleMover.h"
#include "../utils/parallel/parallel_executor.h"
#include "Camera.h"
#include <limits>
#include <algorithm>
#include <vector>
#include "../utils/BVH/TriangleOctree.h"
#define NOMINMAX
#include "../windows/Window.h"


namespace Engine
{

	
	class Scene
	{
	protected:
		enum class IntersectedType { Sphere, Plane, Triangle, Mesh, PointLight, Num };
		struct ObjRef
		{
			void* object{};
			IntersectedType type = IntersectedType::Num;
		};

	public:
		
		std::unique_ptr<IObjectMover> mover;


		Scene(const std::vector<ColoredSphere>& spheres,
			const std::vector<ColoredPlane>& planes,
			const std::vector<ColoredTriangle>& triangles,
			const std::vector<ColorMesh>& meshes,
			const std::vector<ColorDirectionLight>& directionLights,
			const std::vector<ColorPointLight>& pointLights,
			const std::vector<ColorSpotLight>& flashLights
		)
			: matSpheres(spheres)
			, matPlanes(planes)
			, matTriangles(triangles)
			, matMeshes(meshes)
			, directionLights(directionLights)
			, pointLights(pointLights)
			, spotLights(flashLights)
		{
			
			for (auto& mesh : matMeshes)
			{
				TriangleOctree tOctree;
				tOctree.initialize(mesh);

				triangleOctrees.emplace_back(std::move(tOctree));

			}
		}


		void render(const Window& window, std::shared_ptr<Camera> camera, const glm::vec2& mousePoint);
		uint32_t castRayColorToUint(const glm::vec3& rayColor);
		void computeColor(const Window& window,float windowHeight,float windowWidth,float verticalOffset,
			float horizontalOffset,float xNdcCoefficient, const glm::mat4& IPV, int xCapture, int yCapture,
			int x, int y, const glm::vec3& cameraPosition);
		bool findIntersection(const ray& r, Intersection& intersection, ObjRef& obj);
		void getObjectColor(const Intersection& intersection, const ObjRef& obj, glm::vec3& color);
		void setCaptureObj(bool value) { captureObj = value; }
		glm::vec3 castRay(ray& ray, float farPlane, float bgColor, bool captureObjRay);
		void setCapturedObj(const ObjRef& obj, const Intersection& intersection);




	private:
		std::vector<ColoredSphere> matSpheres;
		std::vector<ColoredPlane> matPlanes;
		std::vector<ColoredTriangle> matTriangles;
		std::vector<ColorMesh> matMeshes;
		std::vector<TriangleOctree> triangleOctrees;
		std::vector<ColorDirectionLight> directionLights;
		std::vector<ColorPointLight> pointLights;
		std::vector<ColorSpotLight> spotLights;
		float lenToNearPlane{};
		bool captureObj{};
	};

}
