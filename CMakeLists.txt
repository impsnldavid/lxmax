# Copyright 2020 David Butler. All rights reserved.
# Use of this source code is governed by the MIT License found in the LICENSE file.

cmake_minimum_required(VERSION 3.0)

string(REGEX REPLACE "(.*)/" "" THIS_PACKAGE_NAME "${CMAKE_CURRENT_SOURCE_DIR}")

# Add modules
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules ${CMAKE_MODULE_PATH})

# Get version information
include(GetGitRevisionDescription)
git_describe(VERSION --tags --dirty=-dirty)

# Parse the version information into pieces.
string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_BUGFIX "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SHA1 "${VERSION}")
set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUGFIX}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/version_info.hpp.in
                ${CMAKE_CURRENT_BINARY_DIR}/version_info/version_info.hpp)
				
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/version_info_macros.hpp.in
                ${CMAKE_CURRENT_BINARY_DIR}/version_info/version_info_macros.hpp)
				
include_directories(${CMAKE_CURRENT_BINARY_DIR}/version_info)


if (${CMAKE_GENERATOR} MATCHES "Xcode")
 		if (${XCODE_VERSION} VERSION_LESS 9)
  			message(STATUS "Xcode 9 or higher is required. Please install from the Mac App Store.")
			return ()
		endif ()
endif ()

if(MSVC)
	add_definitions(-DNOMINMAX)
endif()

# Install dependencies
include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

# Misc setup and subroutines
include(${CMAKE_CURRENT_SOURCE_DIR}/source/min-api/script/min-package.cmake)

# Add the Lib, if it exists
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/min-lib/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/min-lib)
endif ()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/lxmax-lib)

# Generate a project for every folder in the "source/projects" folder
SUBDIRLIST(PROJECT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/source/projects)
foreach (project_dir ${PROJECT_DIRS})
	if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/projects/${project_dir}/CMakeLists.txt")
		message("Generating: ${project_dir}")
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/projects/${project_dir})
	endif ()
endforeach ()
