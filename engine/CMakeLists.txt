# add submodules and linking lib + build submodules 'on-fly'
file(GLOB_RECURSE ENGINE_SOURCES "src/*.cpp")
file(GLOB_RECURSE ENGINE_HEADERS "src/*.h")



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


set (IMGUI_SOURCES
    ${DEPENDENCIES_DIR}/imgui/imgui.cpp
    ${DEPENDENCIES_DIR}/imgui/imgui_draw.cpp
    ${DEPENDENCIES_DIR}/imgui/imgui_tables.cpp
    ${DEPENDENCIES_DIR}/imgui/imgui_widgets.cpp
    #"${DEPENDENCIES_DIR}/imgui/backends/imgui_impl_sdl2.cpp"
    #"${DEPENDENCIES_DIR}/imgui/backends/imgui_impl_sdl3.cpp"
    #"${DEPENDENCIES_DIR}/imgui/backends/imgui_impl_sdlrenderer.cpp"
    #"${DEPENDENCIES_DIR}/imgui/backends/imgui_impl_vulkan.cpp"
    #"${DEPENDENCIES_DIR}/imgui/backends/imgui_impl_wgpu.cpp"
    "${DEPENDENCIES_DIR}/imgui/backends/imgui_impl_win32.cpp"
    #"${DEPENDENCIES_DIR}/imgui/backends/imgui_impl_allegro5.cpp"
    #"${DEPENDENCIES_DIR}/imgui/backends/imgui_impl_android.cpp"
    "${DEPENDENCIES_DIR}/imgui/backends/imgui_impl_dx9.cpp"
    "${DEPENDENCIES_DIR}/imgui/backends/imgui_impl_dx10.cpp"
    "${DEPENDENCIES_DIR}/imgui/backends/imgui_impl_dx11.cpp"
    #"${DEPENDENCIES_DIR}/imgui/backends/imgui_impl_dx12.cpp"
    #"${DEPENDENCIES_DIR}/imgui/backends/imgui_impl_glfw.cpp"
    #"${DEPENDENCIES_DIR}/imgui/backends/imgui_impl_glut.cpp"
    #"${DEPENDENCIES_DIR}/imgui/backends/imgui_impl_opengl2.cpp"
    #"${DEPENDENCIES_DIR}/imgui/backends/imgui_impl_opengl3.cpp"
)


#-----------------------------------------------------------------------


 add_library(engine STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS} ${IMGUI_SOURCES})

#link libs -------------------------------------------------------------

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
