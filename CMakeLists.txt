cmake_minimum_required(VERSION 3.11)
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
	add_compile_options(/Os) # tells the compiler to favor optimizations for size over optimizations for speed
	add_compile_options(/O1) # sets a combination of optimizations that generate minimum size code
	add_compile_options(/Ob1) # Allows expansion only of functions marked inline, __inline, or __forceinline, or in a C++ member function defined in a class declaration
	add_compile_options(/Oi) # generates intrinsic functions for appropriate function calls
#	add_compile_options(/Oy) # suppresses the creation of frame pointers on the call stack for quicker function calls
	add_definitions(-D_HAS_EXCEPTIONS=0)
	add_definitions(-D_USE_MATH_DEFINES) # enable macros like M_PI
	add_definitions(-DNOMINMAX) # prevent windows.h from defining min/max macros
	add_compile_options(/GS-) # disable guardstack
	list(APPEND libray_SOURCES src/MyMath-masm-x86.asm)
	list(APPEND libray_SOURCES src/MappedWritableFile-windows.cc)
	list(APPEND libray_SOURCES src/MyLib-windows.cc)
	list(APPEND ray_single_SOURCES src/start-windows.c)
	list(APPEND ray_multi_SOURCES src/start-windows.c)
	find_program(CRINKLER NAMES Crinkler PATHS vendor/)
	file(WRITE "${CMAKE_BINARY_DIR}/Directory.Build.props" "<Project><PropertyGroup><LinkToolExe>${CRINKLER}</LinkToolExe></PropertyGroup></Project>")
else()
	enable_language(ASM)
	add_compile_options(-Wall -Wextra -Wpedantic -Werror)
	add_compile_options(-g)
	add_compile_options(-fno-exceptions -fno-rtti)
	add_compile_options(-fno-asynchronous-unwind-tables) # comment this line while profiling
	add_compile_options(-fno-align-functions)
	add_compile_options(-ffast-math)
	add_compile_options(-fno-stack-protector)

	add_compile_options(-ffunction-sections -fdata-sections)

	list(APPEND libray_SOURCES src/MyMath-inlineasm.cc)
	list(APPEND libray_SOURCES src/MappedWritableFile-posix.cc)
	list(APPEND libray_SOURCES src/MyLib-posix.cc)
	if(APPLE)
		string(REPLACE -Os -Oz CMAKE_CXX_FLAGS_MINSIZEREL ${CMAKE_CXX_FLAGS_MINSIZEREL})
		add_link_options(-Wl,-dead_strip)
		list(APPEND ray_single_SOURCES src/start-x86-64-macos.S)
	else()
		add_compile_options(-fno-align-loops -fno-align-labels -fno-align-jumps)
		add_compile_options(-m32)
		add_compile_options(-fno-pie)
		add_link_options(-m32)
		add_link_options(-Wl,--gc-sections -Wl,--as-needed)
		add_link_options(-Wl,--build-id=none)

		list(APPEND ray_single_SOURCES src/start-i386-linux.S)
		list(APPEND ray_multi_SOURCES src/start-i386-linux.S)
		list(APPEND ray_multi_SOURCES src/thread-i386-linux.S)

		add_link_options(-Wl,-z,noseparate-code) # merge rodata into text
		add_link_options(-Wl,-z,norelro)
	endif()
endif()

set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "") # disable -rdynamic - https://stackoverflow.com/q/35155835

set(DEFAULT_WIDTH "1280" CACHE STRING "Width of the image the raytracer generates")
set(DEFAULT_HEIGHT "800" CACHE STRING "Height of the image the raytracer generates")

option(DO_STRIP "Strip binaries to shave off bytes" ON)

configure_file(src/config.h.in config.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(libray ${libray_SOURCES})

add_executable(ray-single ${ray_single_SOURCES})
target_link_libraries(ray-single libray)

find_package(Threads)

add_executable(ray ${ray_multi_SOURCES})
target_link_libraries(ray libray ${CMAKE_THREAD_LIBS_INIT})

if(MSVC)
	target_link_options(ray PRIVATE /REPORT:ray.html)
	target_link_options(ray-single PRIVATE /REPORT:ray-single.html)
else()
	if(APPLE)
		target_link_options(ray PUBLIC -Wl,-map,ray.map)
		target_link_options(ray-single PUBLIC
			-nostartfiles -nodefaultlibs -nostdlib -static
			-Wl,-map,ray-single.map
		)
		if(DO_STRIP)
			add_custom_command(TARGET ray POST_BUILD
				COMMAND $<IF:$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>,${CMAKE_STRIP},true> ray
				COMMENT "Stripping executable ray"
			)
			add_custom_command(TARGET ray-single POST_BUILD
				COMMAND $<IF:$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>,${CMAKE_STRIP},true> ray-single
				COMMENT "Stripping executable ray-single"
			)
		endif()
	else()
		target_link_options(ray PRIVATE
			-nostartfiles -nodefaultlibs -nolibc -nostdlib -static
			-Wl,-Map,ray.map
		)
		target_link_options(ray-single PRIVATE
			-nostartfiles -nodefaultlibs -nolibc -nostdlib -static
			-Wl,-Map,ray-single.map
		)
		if(DO_STRIP)
			set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor")
			add_custom_command(TARGET ray POST_BUILD
				COMMAND $<IF:$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>,${CMAKE_STRIP},true> -s -R .comment ray
				COMMAND ${VENDOR_DIR}/autovndh.py --gzip --xz --lzma --zstd --zopfli --vndh ${VENDOR_DIR} -- ray ray.vndh
				COMMENT "Minifying executable ray"
			)
			add_custom_command(TARGET ray-single POST_BUILD
				COMMAND $<IF:$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>,${CMAKE_STRIP},true> -s -R .comment ray-single
				COMMAND ${VENDOR_DIR}/autovndh.py --gzip --xz --lzma --zstd --zopfli --vndh ${VENDOR_DIR} -- ray-single ray-single.vndh
				COMMENT "Minifying executable ray-single"
			)
		endif()
	endif()
endif()
