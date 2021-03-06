# Copyright 2020 David Butler. All rights reserved.
# Use of this source code is governed by the MIT License found in the LICENSE file.

cmake_minimum_required(VERSION 3.15)

set(C74_MIN_API_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../min-api)
include(${C74_MIN_API_DIR}/script/min-pretarget.cmake)

include_directories( 
	"${C74_INCLUDES}"
	"${CMAKE_CURRENT_SOURCE_DIR}/../shared"
)

set( SOURCE_FILES
	../shared/common.hpp
	${PROJECT_NAME}.cpp
)

add_library( 
	${PROJECT_NAME} 
	MODULE
	${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	lxmax-lib
)

set_property(TARGET ${PROJECT_NAME} PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

include(${C74_MIN_API_DIR}/script/min-posttarget.cmake)

include(${C74_MIN_API_DIR}/test/min-object-unittest.cmake)
