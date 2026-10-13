# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/workspaces/opengl/build-win/_deps/stb-src"
  "/workspaces/opengl/build-win/_deps/stb-build"
  "/workspaces/opengl/build-win/_deps/stb-subbuild/stb-populate-prefix"
  "/workspaces/opengl/build-win/_deps/stb-subbuild/stb-populate-prefix/tmp"
  "/workspaces/opengl/build-win/_deps/stb-subbuild/stb-populate-prefix/src/stb-populate-stamp"
  "/workspaces/opengl/build-win/_deps/stb-subbuild/stb-populate-prefix/src"
  "/workspaces/opengl/build-win/_deps/stb-subbuild/stb-populate-prefix/src/stb-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/workspaces/opengl/build-win/_deps/stb-subbuild/stb-populate-prefix/src/stb-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/workspaces/opengl/build-win/_deps/stb-subbuild/stb-populate-prefix/src/stb-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
