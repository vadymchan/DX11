#pragma once
#include "../source/math/directionalLightRC.h"
#include "../source/material/coloredSphereRC.h"
#include "../source/material/coloredDirectionLightRC.h"
#include "../source/material/coloredPointLightRC.h"
#include "../source/material/coloredSpotLightRC.h"
#include "../source/material/coloredTriangleRC.h"
#include "../source/material/coloredPlaneRC.h"
#include "../source/material/coloredMeshRC.h"
#include "../source/math/intersectionRC.h"
#include "../utils/Dragger/IObjectMoverRC.h"
#include "../utils/Dragger/SphereMoverRC.h"
#include "../utils/Dragger/MeshMoverRC.h"
#include "../utils/Dragger/PlaneMoverRC.h"
#include "../utils/Dragger/TriangleMoverRC.h"
#include "../utils/parallel/parallel_executorRC.h"
#include "CameraRC.h"
#include <limits>
#include <algorithm>
#include <vector>
#include "../utils/BVH/TriangleOctreeRC.h"
#define NOMINMAX
#include "../windows/WindowRC.h"




namespace RC::engine
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
		void computeColor(const Window& window, float windowHeight, float windowWidth, float verticalOffset,
			float horizontalOffset, float xNdcCoefficient, const glm::mat4& IPV, int xCapture, int yCapture,
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

