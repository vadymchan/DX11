# Install script for directory: C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/DirectXTK-out")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/build/bin/CMake/Debug/DirectXTK.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/build/bin/CMake/Release/DirectXTK.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/build/bin/CMake/MinSizeRel/DirectXTK.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/build/bin/CMake/RelWithDebInfo/DirectXTK.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxtk/DirectXTK-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxtk/DirectXTK-targets.cmake"
         "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/build/CMakeFiles/Export/a11a99d19d8d3c8432b0fa94ef825414/DirectXTK-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxtk/DirectXTK-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/directxtk/DirectXTK-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxtk" TYPE FILE FILES "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/build/CMakeFiles/Export/a11a99d19d8d3c8432b0fa94ef825414/DirectXTK-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxtk" TYPE FILE FILES "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/build/CMakeFiles/Export/a11a99d19d8d3c8432b0fa94ef825414/DirectXTK-targets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxtk" TYPE FILE FILES "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/build/CMakeFiles/Export/a11a99d19d8d3c8432b0fa94ef825414/DirectXTK-targets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxtk" TYPE FILE FILES "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/build/CMakeFiles/Export/a11a99d19d8d3c8432b0fa94ef825414/DirectXTK-targets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxtk" TYPE FILE FILES "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/build/CMakeFiles/Export/a11a99d19d8d3c8432b0fa94ef825414/DirectXTK-targets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/directxtk" TYPE FILE FILES
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/BufferHelpers.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/CommonStates.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/DDSTextureLoader.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/DirectXHelpers.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/Effects.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/GeometricPrimitive.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/GraphicsMemory.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/Model.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/PostProcess.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/PrimitiveBatch.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/ScreenGrab.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/SpriteBatch.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/SpriteFont.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/VertexTypes.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/WICTextureLoader.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/GamePad.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/Keyboard.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/Mouse.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/SimpleMath.h"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/SimpleMath.inl"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source/Inc/Audio.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/directxtk" TYPE FILE FILES
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/build/directxtk-config.cmake"
    "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/build/directxtk-config-version.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
