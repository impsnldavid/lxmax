cmake_minimum_required(VERSION 3.0)

set( HEADER_FILES
	color_component.hpp
	dmx_service.hpp
	dmx_service_config.hpp
	double_buffer.hpp
)

set( SOURCE_FILES
	dmx_service.cpp
)

add_library( 
	lxmax-lib STATIC ${HEADER_FILES} ${SOURCE_FILES}
)

target_include_directories(lxmax-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(lxmax-lib PUBLIC
	CONAN_PKG::Poco
)
