# add submodules and linking lib + build submodules 'on-fly'
set(ENGINE_SOURCES
	"DirectX 11/source/Controller/ModelManager.cpp"
	"DirectX 11/source/Controller/ShaderManager.cpp"
	"DirectX 11/source/D3D/D3D.cpp"
	"DirectX 11/source/Math/Colision/boxDX.cpp"
	"DirectX 11/source/Math/Colision/MeshTriangleOctreeDX.cpp"
	"DirectX 11/source/Math/Colision/ModelTriangleOctreeDX.cpp"
	"DirectX 11/source/MeshSystem/Mesh.cpp"
	"DirectX 11/source/MeshSystem/MeshSystem.cpp"
	"DirectX 11/source/MeshSystem/Model.cpp"
	"DirectX 11/source/MeshSystem/OpaqueInstances.cpp"
	"DirectX 11/source/Texture/DDSTextureLoader11.cpp"
	"DirectX 11/source/Utils/Camera.cpp"
	"DirectX 11/window/WindowDX.cpp"
	"DirectX 11/EngineDX.cpp"
	"general/utils/timer/FPSTimerRC.cpp"
	"general/utils/console/Console.cpp"
	"Ray Casting/render/SceneRC.cpp"
	"Ray Casting/source/math/boxRC.cpp"
	"Ray Casting/source/math/meshRC.cpp"
	"Ray Casting/source/math/planeRC.cpp"
	"Ray Casting/source/math/rayRC.cpp"
	"Ray Casting/source/math/sphereRC.cpp"
	"Ray Casting/source/math/triangleRC.cpp"
	"Ray Casting/utils/BVH/TriangleOctreeRC.cpp"
	"Ray Casting/utils/parallel/parallel_executorRC.cpp"
	"Ray Casting/window/WindowRC.cpp"
)

set(ENGINE_HEADERS
	"DirectX 11/source/Buffer/Buffer.h"
	"DirectX 11/source/Buffer/ConstantBuffer.h"
	"DirectX 11/source/Buffer/DepthStencilBuffer.h"
	"DirectX 11/source/Buffer/IndexBuffer.h"
	"DirectX 11/source/Buffer/VertexBuffer.h"
	"DirectX 11/source/Controller/BufferManager.h"
	"DirectX 11/source/Controller/TextureManager.h"
	"DirectX 11/source/Math/Movement/MeshMover.h"
	"DirectX 11/source/Shader/Shader.h"
	"DirectX 11/source/Shader/VertexShader.h"
	"DirectX 11/source/Shader/HullShader.h"
	"DirectX 11/source/Shader/DomainShader.h"
	"DirectX 11/source/Shader/GeometryShader.h"
	"DirectX 11/source/Shader/PixelShader.h"
	"DirectX 11/source/Texture/SampleState.h"
	"DirectX 11/source/Texture/Texture2D.h"
	"DirectX 11/source/Renderer.h"
	"DirectX 11/EngineDX.h"
	"general/include/SimpleMathToHLSL.h"
	"general/include/win.h"
	"general/include/win_def.h"
	"general/include/win_undef.h"
	"Ray Casting/source/material/coloredMeshRC.h"
	"Ray Casting/source/material/coloredPlaneRC.h"
	"Ray Casting/source/material/coloredPointLightRC.h"
	"Ray Casting/source/material/coloredSphereRC.h"
	"Ray Casting/source/material/coloredSpotLightRC.h"
	"Ray Casting/source/material/coloredTriangleRC.h"
	"Ray Casting/source/material/materialRC.h"
	"Ray Casting/source/material/coloredDirectionLightRC.h"
	"Ray Casting/utils/Camera.h"
	"Ray Casting/utils/Dragger/IObjectMoverRC.h"
	"Ray Casting/utils/Dragger/MeshMoverRC.h"
	"Ray Casting/utils/Dragger/PlaneMoverRC.h"
	"Ray Casting/utils/Dragger/SphereMoverRC.h"
	"Ray Casting/utils/Dragger/TriangleMoverRC.h"
	"DirectX 11/source/Controller/ModelManager.h"
	"DirectX 11/source/Controller/ShaderManager.h"
	"DirectX 11/source/D3D/D3D.h"
	"DirectX 11/source/Math/Colision/boxDX.h"
	"DirectX 11/source/Math/Colision/MeshTriangleOctreeDX.h"
	"DirectX 11/source/Math/Colision/ModelTriangleOctreeDX.h"
	"DirectX 11/source/MeshSystem/Mesh.h"
	"DirectX 11/source/MeshSystem/MeshSystem.h"
	"DirectX 11/source/MeshSystem/Model.h"
	"DirectX 11/source/MeshSystem/OpaqueInstances.h"
	"DirectX 11/source/Texture/DDSTextureLoader11.h"
	"DirectX 11/source/Utils/Camera.h"
	"DirectX 11/window/WindowDX.h"
	"DirectX 11/EngineDX.h"
	"general/utils/timer/FPSTimerRC.h"
	"general/utils/console/Console.h"
	"Ray Casting/render/SceneRC.h"
	"Ray Casting/source/math/boxRC.h"
	"Ray Casting/source/math/meshRC.h"
	"Ray Casting/source/math/planeRC.h"
	"Ray Casting/source/math/rayRC.h"
	"Ray Casting/source/math/sphereRC.h"
	"Ray Casting/source/math/triangleRC.h"
	"Ray Casting/utils/BVH/TriangleOctreeRC.h"
	"Ray Casting/utils/parallel/parallel_executorRC.h"
	"Ray Casting/window/WindowRC.h"
)



include(ExternalProject)

set(DEPENDENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/general/dependencies)

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

ExternalProject_Add(glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 0.9.9.8
  PREFIX ${DEPENDENCIES_DIR}/glm/glm-prefix
  SOURCE_DIR ${DEPENDENCIES_DIR}/glm/source
  BINARY_DIR ${DEPENDENCIES_DIR}/glm/build
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=$${DEPENDENCIES_DIR}/glm/glm-out
    -DGLM_TEST_ENABLE=OFF
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
  UPDATE_DISCONNECTED ON
)
set_target_properties(glm PROPERTIES EXCLUDE_FROM_ALL TRUE)

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
add_dependencies(engine glm)
target_include_directories(engine SYSTEM PUBLIC ${DEPENDENCIES_DIR}/glm/source)

if (${CMAKE_GENERATOR_PLATFORM} "x64")
    set(build_architecture "x64")
endif()

# Copy Assimp DLL to the directory where the exe is located
add_custom_command(TARGET engine POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    ${DEPENDENCIES_DIR}/assimp/assimp-out/bin/assimp-vc143-mtd.dll
    ${CMAKE_BINARY_DIR}
)




#target_link_directories( engine PUBLIC
#    "${CMAKE_CURRENT_SOURCE_DIR}/general/dependencies/DirectXTK/Bin/Desktop_2022/x64/Debug"
#    "${CMAKE_CURRENT_SOURCE_DIR}/general/dependencies/assimp/lib/Debug"
#)

#target_link_libraries(engine PRIVATE
#    assimp-vc143-mtd.lib
#    DirectXTK.lib
#)

target_compile_features(engine PRIVATE cxx_std_17)
