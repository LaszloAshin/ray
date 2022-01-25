cmake_minimum_required(VERSION 3.0)
project(ray VERSION 0.0.1 LANGUAGES CXX)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(libray_SOURCES
	src/BaseObject.cc
	src/Color.cc
	src/Geometry.cc
	src/Image.cc
	src/Light.cc
	src/MyLib.cc
	src/Plane.cc
	src/Scene.cc
	src/Sphere.cc
	src/Spheroid.cc
	src/Tracer.cc
	src/Vector.cc
)

if(MSVC)
	enable_language(ASM_MASM)
	add_compile_options(/W4 /WX) # highest warning level
	add_compile_options(/EHs-c- /GR-) # disable exceptions and rtti
	add_definitions(-D_HAS_EXCEPTIONS=0)
	add_definitions(-D_USE_MATH_DEFINES) # enable macros like M_PI
	add_definitions(-DNOMINMAX) # prevent windows.h from defining min/max macros
	add_compile_options(/GS-) # disable guardstack
	add_link_options(/MANIFEST:NO)
	list(APPEND libray_SOURCES src/MyMath-masm.asm)
	list(APPEND libray_SOURCES src/MappedWritableFile-windows.cc)
else()
	add_compile_options(-Wall -Wextra -Wpedantic -Werror)
	add_compile_options(-fno-exceptions -fno-rtti)
	add_compile_options(-fno-asynchronous-unwind-tables)
	add_compile_options(-fno-align-functions)
	add_compile_options(-ffast-math)

	add_compile_options(-ffunction-sections -fdata-sections)

	list(APPEND libray_SOURCES src/MyMath-inlineasm.cc)
	list(APPEND libray_SOURCES src/MappedWritableFile-posix.cc)
	if(APPLE)
		add_link_options(-Wl,-dead_strip)
	else()
		enable_language(ASM)
		add_compile_options(-fno-align-loops)
		add_link_options(-Wl,--gc-sections -Wl,--as-needed)
		add_link_options(-Wl,--build-id=none)

		add_link_options(-nostartfiles)
		list(APPEND libray_SOURCES src/start-x86-64-linux.s)

		add_link_options(-Wl,-znoseparate-code) # merge rodata into text
	endif()
endif()

set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "") # disable -rdynamic - https://stackoverflow.com/q/35155835

set(DEFAULT_WIDTH "1280" CACHE STRING "Width of the image the raytracer generates")
set(DEFAULT_HEIGHT "800" CACHE STRING "Height of the image the raytracer generates")

configure_file(src/config.h.in config.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(libray ${libray_SOURCES})

add_executable(ray-single src/MainSingle.cc)
target_link_libraries(ray-single libray)

find_package(Threads)

add_executable(ray src/MainMulti.cc)
target_link_libraries(ray libray ${CMAKE_THREAD_LIBS_INIT})
