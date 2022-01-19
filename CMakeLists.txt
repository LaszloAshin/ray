cmake_minimum_required(VERSION 3.0)
project(ray VERSION 0.0.1)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
	add_compile_options(/W4 /WX)
	add_compile_options(/GX- /GR-)
else()
	add_compile_options(-Wall -Wextra -Wpedantic -Werror)
	add_compile_options(-fno-exceptions -fno-rtti)
	add_compile_options(-fno-asynchronous-unwind-tables)
	add_compile_options(-fno-align-functions)

	add_compile_options(-ffunction-sections -fdata-sections)
	if(APPLE)
		add_link_options(-Wl,-dead_strip)
	else()
		add_link_options(-Wl,--gc-sections -Wl,--as-needed)
	endif()
endif()

add_definitions(-D_USE_MATH_DEFINES)

set(DEFAULT_WIDTH "1280" CACHE STRING "Width of the image the raytracer generates")
set(DEFAULT_HEIGHT "800" CACHE STRING "Height of the image the raytracer generates")

configure_file(src/config.h.in config.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(libray
	src/BaseObject.cc
	src/Color.cc
	src/Geometry.cc
	src/Image.cc
	src/Light.cc
	src/Plane.cc
	src/Scene.cc
	src/Spheroid.cc
	src/Tracer.cc
	src/Vector.cc
)

add_executable(ray-single src/MainSingle.cc)
target_link_libraries(ray-single libray)

find_package(Threads)

add_executable(ray src/MainMulti.cc)
target_link_libraries(ray libray ${CMAKE_THREAD_LIBS_INIT})
