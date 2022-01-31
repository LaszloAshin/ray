cmake_minimum_required(VERSION 3.0)
project(ray VERSION 0.0.1 LANGUAGES C CXX)
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

set(ray_single_SOURCES src/MainSingle.cc)
set(ray_multi_SOURCES src/MainMulti.cc)

if(MSVC)
	enable_language(ASM_MASM)
	add_compile_options(/W4 /WX) # highest warning level
	add_compile_options(/EHs-c- /GR-) # disable exceptions and rtti
	add_definitions(-D_HAS_EXCEPTIONS=0)
	add_definitions(-D_USE_MATH_DEFINES) # enable macros like M_PI
	add_definitions(-DNOMINMAX) # prevent windows.h from defining min/max macros
	add_compile_options(/GS-) # disable guardstack
	add_link_options(/MANIFEST:NO)
	add_link_options(/MERGE:.rdata=.text)
	add_link_options(/MERGE:.pdata=.text)
	add_link_options(/MAP)
	list(APPEND libray_SOURCES src/MyMath-masm.asm)
	list(APPEND libray_SOURCES src/MappedWritableFile-windows.cc)
	list(APPEND libray_SOURCES src/MyLib-windows.cc)
	list(APPEND ray_single_SOURCES src/start-windows.c)
else()
	add_compile_options(-Wall -Wextra -Wpedantic -Werror)
	add_compile_options(-fno-exceptions -fno-rtti)
	add_compile_options(-fno-asynchronous-unwind-tables)
	add_compile_options(-fno-align-functions)
	add_compile_options(-ffast-math)
	add_compile_options(-fno-stack-protector)

	add_compile_options(-ffunction-sections -fdata-sections)

	list(APPEND libray_SOURCES src/MyMath-inlineasm.cc)
	list(APPEND libray_SOURCES src/MappedWritableFile-posix.cc)
	list(APPEND libray_SOURCES src/MyLib-posix.cc)
	if(APPLE)
		add_link_options(-Wl,-dead_strip)
	else()
		enable_language(ASM)
		add_compile_options(-fno-align-loops -fno-align-labels -fno-align-jumps)
		add_link_options(-Wl,--gc-sections -Wl,--as-needed)
		add_link_options(-Wl,--build-id=none)

		add_link_options(-nostartfiles -nodefaultlibs -nolibc -nostdlib -static)
		list(APPEND libray_SOURCES src/start-x86-64-linux.s)
		list(APPEND ray_multi_SOURCES src/thread-linux.s)

		add_link_options(-Wl,-z,noseparate-code) # merge rodata into text
		add_link_options(-Wl,-z,norelro)
	endif()
endif()

set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "") # disable -rdynamic - https://stackoverflow.com/q/35155835

set(DEFAULT_WIDTH "1280" CACHE STRING "Width of the image the raytracer generates")
set(DEFAULT_HEIGHT "800" CACHE STRING "Height of the image the raytracer generates")

configure_file(src/config.h.in config.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(libray ${libray_SOURCES})

add_executable(ray-single ${ray_single_SOURCES})
target_link_libraries(ray-single libray)

find_package(Threads)

add_executable(ray ${ray_multi_SOURCES})
target_link_libraries(ray libray ${CMAKE_THREAD_LIBS_INIT})

if(UNIX)
	if(NOT APPLE)
		add_custom_command(TARGET ray POST_BUILD
			COMMAND $<IF:$<CONFIG:Release>,${CMAKE_STRIP},true> -s -R .comment ray
			COMMENT "Stripping executable ray"
		)
		add_custom_command(TARGET ray-single POST_BUILD
			COMMAND $<IF:$<CONFIG:Release>,${CMAKE_STRIP},true> -s -R .comment ray-single
			COMMENT "Stripping executable ray-single"
		)
	endif()
	target_link_options(ray PUBLIC -Wl,-Map=ray.map)
	target_link_options(ray-single PUBLIC -Wl,-Map=ray-single.map)
endif()
