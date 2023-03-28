# add submodules and linking lib + build submodules 'on-fly'
set(ENGINE_SOURCES
	"src/DirectX 11/source/Controller/ModelManager.cpp"
	"src/DirectX 11/source/Controller/ShaderManager.cpp"
	"src/DirectX 11/source/D3D/D3D.cpp"
	"src/DirectX 11/source/Math/Colision/boxDX.cpp"
	"src/DirectX 11/source/Math/Colision/MeshTriangleOctreeDX.cpp"
	"src/DirectX 11/source/Math/Colision/ModelTriangleOctreeDX.cpp"
	"src/DirectX 11/source/MeshSystem/Mesh.cpp"
	"src/DirectX 11/source/MeshSystem/MeshSystem.cpp"
	"src/DirectX 11/source/MeshSystem/Model.cpp"
	"src/DirectX 11/source/MeshSystem/OpaqueInstances.cpp"
	"src/DirectX 11/source/Texture/DDSTextureLoader11.cpp"
	"src/DirectX 11/source/Utils/Camera.cpp"
	"src/DirectX 11/window/WindowDX.cpp"
	"src/DirectX 11/EngineDX.cpp"
	"src/general/utils/timer/FPSTimerRC.cpp"
	"src/general/utils/console/Console.cpp"
	"src/Ray Casting/render/SceneRC.cpp"
	"src/Ray Casting/source/math/boxRC.cpp"
	"src/Ray Casting/source/math/meshRC.cpp"
	"src/Ray Casting/source/math/planeRC.cpp"
	"src/Ray Casting/source/math/rayRC.cpp"
	"src/Ray Casting/source/math/sphereRC.cpp"
	"src/Ray Casting/source/math/triangleRC.cpp"
	"src/Ray Casting/utils/BVH/TriangleOctreeRC.cpp"
	"src/Ray Casting/utils/parallel/parallel_executorRC.cpp"
	"src/Ray Casting/window/WindowRC.cpp"
)

set(ENGINE_HEADERS
	"src/DirectX 11/source/Buffer/Buffer.h"
	"src/DirectX 11/source/Buffer/ConstantBuffer.h"
	"src/DirectX 11/source/Buffer/DepthStencilBuffer.h"
	"src/DirectX 11/source/Buffer/IndexBuffer.h"
	"src/DirectX 11/source/Buffer/VertexBuffer.h"
	"src/DirectX 11/source/Controller/BufferManager.h"
	"src/DirectX 11/source/Controller/TextureManager.h"
	"src/DirectX 11/source/Math/Movement/MeshMover.h"
	"src/DirectX 11/source/Shader/Shader.h"
	"src/DirectX 11/source/Shader/VertexShader.h"
	"src/DirectX 11/source/Shader/HullShader.h"
	"src/DirectX 11/source/Shader/DomainShader.h"
	"src/DirectX 11/source/Shader/GeometryShader.h"
	"src/DirectX 11/source/Shader/PixelShader.h"
	"src/DirectX 11/source/Texture/SampleState.h"
	"src/DirectX 11/source/Texture/Texture2D.h"
	"src/DirectX 11/source/Renderer.h"
	"src/DirectX 11/EngineDX.h"
	"src/general/include/SimpleMathToHLSL.h"
	"src/general/include/win.h"
	"src/general/include/win_def.h"
	"src/general/include/win_undef.h"
	"src/Ray Casting/source/material/coloredMeshRC.h"
	"src/Ray Casting/source/material/coloredPlaneRC.h"
	"src/Ray Casting/source/material/coloredPointLightRC.h"
	"src/Ray Casting/source/material/coloredSphereRC.h"
	"src/Ray Casting/source/material/coloredSpotLightRC.h"
	"src/Ray Casting/source/material/coloredTriangleRC.h"
	"src/Ray Casting/source/material/materialRC.h"
	"src/Ray Casting/source/material/coloredDirectionLightRC.h"
	"src/Ray Casting/utils/Camera.h"
	"src/Ray Casting/utils/Dragger/IObjectMoverRC.h"
	"src/Ray Casting/utils/Dragger/MeshMoverRC.h"
	"src/Ray Casting/utils/Dragger/PlaneMoverRC.h"
	"src/Ray Casting/utils/Dragger/SphereMoverRC.h"
	"src/Ray Casting/utils/Dragger/TriangleMoverRC.h"
	"src/DirectX 11/source/Controller/ModelManager.h"
	"src/DirectX 11/source/Controller/ShaderManager.h"
	"src/DirectX 11/source/D3D/D3D.h"
	"src/DirectX 11/source/Math/Colision/boxDX.h"
	"src/DirectX 11/source/Math/Colision/MeshTriangleOctreeDX.h"
	"src/DirectX 11/source/Math/Colision/ModelTriangleOctreeDX.h"
	"src/DirectX 11/source/MeshSystem/Mesh.h"
	"src/DirectX 11/source/MeshSystem/MeshSystem.h"
	"src/DirectX 11/source/MeshSystem/Model.h"
	"src/DirectX 11/source/MeshSystem/OpaqueInstances.h"
	"src/DirectX 11/source/Texture/DDSTextureLoader11.h"
	"src/DirectX 11/source/Utils/Camera.h"
	"src/DirectX 11/window/WindowDX.h"
	"src/DirectX 11/EngineDX.h"
	"src/general/utils/timer/FPSTimerRC.h"
	"src/general/utils/console/Console.h"
	"src/Ray Casting/render/SceneRC.h"
	"src/Ray Casting/source/math/boxRC.h"
	"src/Ray Casting/source/math/meshRC.h"
	"src/Ray Casting/source/math/planeRC.h"
	"src/Ray Casting/source/math/rayRC.h"
	"src/Ray Casting/source/math/sphereRC.h"
	"src/Ray Casting/source/math/triangleRC.h"
	"src/Ray Casting/utils/BVH/TriangleOctreeRC.h"
	"src/Ray Casting/utils/parallel/parallel_executorRC.h"
	"src/Ray Casting/window/WindowRC.h"
)



include(ExternalProject)

set(DEPENDENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/general/dependencies)

ExternalProject_Add(assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG v5.0.1
  PREFIX ${DEPENDENCIES_DIR}/assimp/assimp-prefix
  SOURCE_DIR ${DEPENDENCIES_DIR}/assimp/source
  BINARY_DIR ${DEPENDENCIES_DIR}/assimp/build
  CMAKE_ARGS
	-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
	-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_DIR}/assimp/assimp-out
    -DASSIMP_BUILD_TESTS=OFF
    -DASSIMP_BUILD_ASSIMP_TOOLS=OFF
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --target install
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
  UPDATE_DISCONNECTED ON
)
set_target_properties(assimp PROPERTIES EXCLUDE_FROM_ALL TRUE)

ExternalProject_Add(DirectXTK
  GIT_REPOSITORY https://github.com/microsoft/DirectXTK.git
  GIT_TAG feb2023
  PREFIX ${DEPENDENCIES_DIR}/DirectXTK/DirectXTK-prefix
  SOURCE_DIR ${DEPENDENCIES_DIR}/DirectXTK/source
  BINARY_DIR ${DEPENDENCIES_DIR}/DirectXTK/build
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_DIR}/DirectXTK/DirectXTK-out
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --target install
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
)
set_target_properties(DirectXTK PROPERTIES EXCLUDE_FROM_ALL TRUE)

include(FetchContent)

# Set the location of the FETCHCONTENT_BASE_DIR variable to a custom directory
set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}/fetchcontent")

# glm ----------------------------------------------------------------------



FetchContent_Declare(glm 
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 0.9.9.8
  SOURCE_DIR ${DEPENDENCIES_DIR}/glm
)




# Fetch the glm external project
FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
  FetchContent_Populate(glm)
endif()


# Fetch the external project
#FetchContent_MakeAvailable(glm)



#imgui -----------------------------------------------------------------

FetchContent_Declare(imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.89.4
  SOURCE_DIR ${DEPENDENCIES_DIR}/imgui
)

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
	FetchContent_Populate(imgui)
endif()


add_library(engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# Link with Assimp
add_dependencies(engine assimp)
target_link_libraries(engine PRIVATE ${DEPENDENCIES_DIR}/assimp/assimp-out/lib/assimp-vc143-mtd.lib)
target_include_directories(engine SYSTEM PUBLIC ${DEPENDENCIES_DIR}/assimp/assimp-out/include)

# Link with DirectX Tool Kit (SimpleMath)
add_dependencies(engine DirectXTK)
target_link_libraries(engine PRIVATE ${DEPENDENCIES_DIR}/DirectXTK/DirectXTK-out/lib/DirectXTK.lib)
target_include_directories(engine SYSTEM PUBLIC ${DEPENDENCIES_DIR}/DirectXTK/DirectXTK-out/include/directxtk)

# Link with GLM
target_include_directories(engine SYSTEM PUBLIC ${DEPENDENCIES_DIR}/glm)

# Link with imgui
target_include_directories(engine SYSTEM PUBLIC ${DEPENDENCIES_DIR}/imgui)


if (${CMAKE_GENERATOR_PLATFORM} "x64")
    set(build_architecture "x64")
endif()

# Copy Assimp DLL to the directory where the exe is located
add_custom_command(TARGET engine POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    ${DEPENDENCIES_DIR}/assimp/assimp-out/bin/assimp-vc143-mtd.dll
    ${CMAKE_BINARY_DIR}
)

target_compile_features(engine PRIVATE cxx_std_17)
