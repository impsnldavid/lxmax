# Copyright 2020 David Butler. All rights reserved.
# Use of this source code is governed by the MIT License found in the LICENSE file.

cmake_minimum_required(VERSION 3.0)

set(C74_BUILD_MAX_EXTENSION TRUE)

set(C74_MIN_API_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../min-api)
include(${C74_MIN_API_DIR}/script/min-pretarget.cmake)

include_directories( 
	"${C74_INCLUDES}"
	"${CMAKE_CURRENT_SOURCE_DIR}/../shared"
)

set( HEADER_FILES
	dict_edit.hpp
	max_console_channel.hpp
)

set( SOURCE_FILES
	${PROJECT_NAME}.cpp
)

add_library( 
	${PROJECT_NAME} 
	MODULE
	${HEADER_FILES}
	${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	lxmax-lib
)

include(${C74_MIN_API_DIR}/script/min-posttarget.cmake)

include(${C74_MIN_API_DIR}/test/min-object-unittest.cmake)
