# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/source"
  "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/build"
  "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/DirectXTK-prefix"
  "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/DirectXTK-prefix/tmp"
  "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/DirectXTK-prefix/src/DirectXTK-stamp"
  "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/DirectXTK-prefix/src"
  "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/DirectXTK-prefix/src/DirectXTK-stamp"
)

set(configSubDirs Debug;Release;MinSizeRel;RelWithDebInfo)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/DirectXTK-prefix/src/DirectXTK-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/vadym/source/repos/vchan/engine/src/general/dependencies/DirectXTK/DirectXTK-prefix/src/DirectXTK-stamp${cfgdir}") # cfgdir has leading slash
endif()
