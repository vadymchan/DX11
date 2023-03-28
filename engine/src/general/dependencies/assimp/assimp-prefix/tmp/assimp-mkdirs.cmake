# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/assimp/source"
  "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/assimp/build"
  "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/assimp/assimp-prefix"
  "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/assimp/assimp-prefix/tmp"
  "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/assimp/assimp-prefix/src/assimp-stamp"
  "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/assimp/assimp-prefix/src"
  "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/assimp/assimp-prefix/src/assimp-stamp"
)

set(configSubDirs Debug;Release;MinSizeRel;RelWithDebInfo)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/assimp/assimp-prefix/src/assimp-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/assimp/assimp-prefix/src/assimp-stamp${cfgdir}") # cfgdir has leading slash
endif()
