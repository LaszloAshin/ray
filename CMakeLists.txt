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
	add_compile_options(-fno-exceptions $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
	add_compile_options(-fno-unwind-tables -fno-asynchronous-unwind-tables) # comment this line while profiling
	add_compile_options(-fno-align-functions)
	add_compile_options(-ffast-math)
	add_compile_options(-fno-stack-protector)
	add_compile_options(-fno-pie)

	add_compile_options(-ffunction-sections -fdata-sections)

	list(APPEND libray_SOURCES src/MyMath-inlineasm.cc)
	list(APPEND libray_SOURCES src/MappedWritableFile-posix.cc)
	list(APPEND libray_SOURCES src/MyLib-posix.cc)
	if(APPLE)
		set(CMAKE_CXX_LINK_FLAGS "") # Avoid cmake to add the flags -search_paths_first and -headerpad_max_install_names
		add_link_options(-Wl,-dead_strip)
		add_link_options(-Wl,-dead_strip_dylibs)
		add_link_options(-Wl,-no_pie)
		add_link_options(-Wl,-no_uuid)
		add_link_options(-Wl,-no_function_starts)
		add_link_options(-Wl,-no_eh_labels)
		add_link_options(-Wl,-unexported_symbol,*)
		list(APPEND libray_SOURCES src/start-x86-64-macos.S)
		list(APPEND ray_multi_SOURCES src/thread-x86-64-macos.S)
	else()
		add_compile_options(-fno-align-loops -fno-align-labels -fno-align-jumps)
		add_compile_options(-m32)
		add_link_options(-m32)
		add_link_options(-Wl,--gc-sections -Wl,--as-needed)
		add_link_options(-Wl,--build-id=none)

		list(APPEND libray_SOURCES src/start-i386-linux.S)
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
		target_link_options(ray PUBLIC
			-nostartfiles -nodefaultlibs -nostdlib -static
			-Wl,-map,ray.map -Wl,-order_file,${CMAKE_SOURCE_DIR}/src/macos-link-order
		)
		target_link_options(ray-single PUBLIC
			-nostartfiles -nodefaultlibs -nostdlib -static
			-Wl,-map,ray-single.map -Wl,-order_file,${CMAKE_SOURCE_DIR}/src/macos-link-order
		)
		if(DO_STRIP)
			include(ExternalProject)
			ExternalProject_Add(onekpaq
				GIT_REPOSITORY https://github.com/LaszloAshin/oneKpaq.git
				GIT_TAG 1e5342b8849ea8a4acbfc53f2288061bde9495c2
				UPDATE_DISCONNECTED ON
				PATCH_COMMAND
					COMMAND ${CMAKE_COMMAND} -E make_directory obj
				CONFIGURE_COMMAND ""
				BUILD_IN_SOURCE ON
				INSTALL_COMMAND
					COMMAND ${CMAKE_COMMAND} -E copy onekpaq ${CMAKE_BINARY_DIR}
			)
			configure_file(src/onekpaq_context.cache.macos ${CMAKE_BINARY_DIR}/onekpaq_context.cache COPYONLY)
			add_executable(compact-macho src/compact-macho.c)
			add_dependencies(compact-macho onekpaq)
			add_dependencies(ray compact-macho)
			add_dependencies(ray-single compact-macho)
			add_custom_command(TARGET ray POST_BUILD
				COMMAND $<IF:$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>,compact-macho,true> ray
				COMMAND chmod +x ray.compact
				COMMENT "Compacting executable ray"
			)
			add_custom_command(TARGET ray-single POST_BUILD
				COMMAND $<IF:$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>,compact-macho,true> ray-single
				COMMAND chmod +x ray-single.compact
				COMMENT "Compacting executable ray-single"
			)
		endif()
	else()
		target_link_options(ray PRIVATE
			-nostartfiles -nodefaultlibs -nolibc -nostdlib -static
			-Wl,-Map,ray.map -Wl,--omagic
		)
		target_link_options(ray-single PRIVATE
			-nostartfiles -nodefaultlibs -nolibc -nostdlib -static
			-Wl,-Map,ray-single.map -Wl,--omagic
		)
		if(DO_STRIP)
			include(ExternalProject)
			ExternalProject_Add(onekpaq
				GIT_REPOSITORY https://github.com/LaszloAshin/oneKpaq.git
				GIT_TAG 1e5342b8849ea8a4acbfc53f2288061bde9495c2
				UPDATE_DISCONNECTED ON
				CONFIGURE_COMMAND ""
				BUILD_IN_SOURCE ON
				INSTALL_COMMAND
					COMMAND ${CMAKE_COMMAND} -E copy onekpaq ${CMAKE_BINARY_DIR}
			)
			configure_file(src/onekpaq_context.cache.linux ${CMAKE_BINARY_DIR}/onekpaq_context.cache COPYONLY)
			add_executable(compact-elf src/compact-elf.c)
			add_dependencies(compact-elf onekpaq)
			target_compile_options(compact-elf PRIVATE -Wno-unused-value -Wno-unused-result)
			add_dependencies(ray compact-elf)
			add_dependencies(ray-single compact-elf)
			add_custom_command(TARGET ray POST_BUILD
				COMMAND $<IF:$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>,compact-elf,true> ray
				COMMENT "Compacting executable ray"
			)
			add_custom_command(TARGET ray-single POST_BUILD
				COMMAND $<IF:$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>,compact-elf,true> ray-single
				COMMENT "Compacting executable ray-single"
			)
		endif()
	endif()
endif()
