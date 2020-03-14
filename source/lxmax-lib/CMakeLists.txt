# Copyright 2020 David Butler. All rights reserved.
# Use of this source code is governed by the MIT License found in the LICENSE file.

cmake_minimum_required(VERSION 3.0)

if (WIN32)
    set(CMAKE_C_FLAGS_DEBUG            "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
    set(CMAKE_C_FLAGS_MINSIZEREL       "/MT /O1 /Ob1 /D NDEBUG")
    set(CMAKE_C_FLAGS_RELEASE          "/MT /O2 /Ob2 /D NDEBUG")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO   "/MT /Zi /O2 /Ob1 /D NDEBUG")

    set(CMAKE_CXX_FLAGS_DEBUG          "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
    set(CMAKE_CXX_FLAGS_MINSIZEREL     "/MT /O1 /Ob1 /D NDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE        "/MT /O2 /Ob2 /D NDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT /Zi /O2 /Ob1 /D NDEBUG")
endif ()

project(lxmax-lib)

set( HEADER_FILES
	dmx_channel_range.hpp
	dmx_universe_config.hpp
	dmx_write_manager.hpp
	color_component.hpp
	color_personality.hpp
	common.hpp
	config_helpers.hpp
	dmx_packet_artnet.hpp
	dmx_packet_sacn.hpp
	dmx_output_service.hpp
	endian_helpers.hpp
	fixture.hpp
	fixture_manager.hpp
	fixture_patch_info.hpp
	global_config.hpp
	hash_functions.hpp
	preferences_manager.hpp
)

set( SOURCE_FILES
	config_helpers.cpp
	dmx_output_service.cpp
	dmx_universe_config.cpp
	fixture.cpp
	fixture_manager.cpp
)

add_library( 
	lxmax-lib STATIC ${HEADER_FILES} ${SOURCE_FILES}
)

target_include_directories(lxmax-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(lxmax-lib PUBLIC
	CONAN_PKG::poco
)
