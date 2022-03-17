cmake_minimum_required(VERSION 3.11)
project(ray VERSION 0.0.1 LANGUAGES C CXX)
enable_testing()
message(STATUS "cmake version: ${CMAKE_VERSION}")

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
	file(DOWNLOAD
		https://github.com/runestubbe/Crinkler/releases/download/v2.3/crinkler23.zip
		crinkler.zip
		SHOW_PROGRESS
		EXPECTED_HASH SHA256=a0456c53cdd754e4cb710649760c0f9459285da684b9304a18d717d3803c42d6
	)
	file(GLOB X ${CMAKE_BINARY_DIR}/*)
	message(STATUS "Files in CMAKE_BINARY_DIR: ${X}")
	file(ARCHIVE_EXTRACT INPUT crinkler.zip VERBOSE)
	find_program(CRINKLER NAMES Crinkler PATHS ${CMAKE_BINARY_DIR}/crinkler23/Win32 REQUIRED)
	message(STATUS "Found crinkler: ${CRINKLER}")
	file(WRITE "${CMAKE_BINARY_DIR}/Directory.Build.props" "<Project><PropertyGroup><LinkToolExe>${CRINKLER}</LinkToolExe></PropertyGroup></Project>")
else()
	enable_language(ASM)
	list(APPEND libray_SOURCES src/MyMath-inlineasm.cc)
	list(APPEND libray_SOURCES src/MappedWritableFile-posix.cc)
	list(APPEND libray_SOURCES src/MyLib-posix.cc)
	if(APPLE)
		list(APPEND libray_SOURCES src/start-x86-64-macos.S)
		list(APPEND ray_multi_SOURCES src/thread-x86-64-macos.S)
	else()
		list(APPEND libray_SOURCES src/start-i386-linux.S)
		list(APPEND ray_multi_SOURCES src/thread-i386-linux.S)
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

add_executable(ray ${ray_multi_SOURCES})
target_link_libraries(ray libray)

if(MSVC)
	target_link_options(ray PRIVATE /REPORT:ray.html)
	target_link_options(ray-single PRIVATE /REPORT:ray-single.html)
else()
	add_compile_options(-Wall -Wextra -Wpedantic -Werror)
	target_compile_options(libray PUBLIC
		-g
		-fno-exceptions $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
		-fno-unwind-tables -fno-asynchronous-unwind-tables # comment this line while profiling
		-fno-align-functions
		-ffast-math
		-fno-stack-protector
		-fno-pie
		-ffunction-sections -fdata-sections
	)

	if(DO_STRIP)
		include(ExternalProject)
		ExternalProject_Add(onekpaq
			GIT_REPOSITORY https://github.com/LaszloAshin/oneKpaq.git
			GIT_TAG 93aaebfd7933a385f4f95d173ee7e03232c9a453
			UPDATE_DISCONNECTED ON
			CONFIGURE_COMMAND ""
			BUILD_IN_SOURCE ON
			INSTALL_COMMAND
				COMMAND ${CMAKE_COMMAND} -E copy onekpaq ${CMAKE_BINARY_DIR}
		)
	endif()
	if(APPLE)
		string(REPLACE -Os -Oz CMAKE_CXX_FLAGS_MINSIZEREL ${CMAKE_CXX_FLAGS_MINSIZEREL})
		set(CMAKE_CXX_LINK_FLAGS "") # Avoid cmake to add the flags -search_paths_first and -headerpad_max_install_names
		target_link_options(libray INTERFACE
			-nostartfiles -nodefaultlibs -nostdlib -static
			-Wl,-order_file,${CMAKE_SOURCE_DIR}/src/macos-link-order
			-Wl,-dead_strip
			-Wl,-dead_strip_dylibs
			-Wl,-no_pie
			-Wl,-no_uuid
			-Wl,-no_function_starts
			-Wl,-no_eh_labels
			-Wl,-unexported_symbol,*
			-Wl,-pagezero_size,0x20000
		)
		target_link_options(ray PRIVATE -Wl,-map,ray.map)
		target_link_options(ray-single PRIVATE -Wl,-map,ray-single.map)
		if(DO_STRIP)
			configure_file(src/onekpaq_context.cache.macos ${CMAKE_BINARY_DIR}/onekpaq_context.cache COPYONLY)
			add_executable(compact-macho src/compact-macho.c)
			add_dependencies(compact-macho onekpaq)
			add_dependencies(ray compact-macho)
			add_dependencies(ray-single compact-macho)
			add_custom_command(TARGET ray POST_BUILD
				COMMAND $<IF:$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>,compact-macho,true> ray
				COMMENT "Compacting executable ray"
			)
			add_custom_command(TARGET ray-single POST_BUILD
				COMMAND $<IF:$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>,compact-macho,true> ray-single
				COMMENT "Compacting executable ray-single"
			)
		endif()
	else()
		target_compile_options(libray PUBLIC
			-m32 -mregparm=3
			-fno-align-loops -fno-align-labels -fno-align-jumps
		)
		target_link_options(libray INTERFACE
			-m32
			-Wl,--gc-sections -Wl,--as-needed
			-Wl,--build-id=none
			-Wl,-z,noseparate-code # merge rodata into text
			-Wl,-z,norelro
			-Wl,-Ttext,0x20074 # 0x74 = Ehdr + 1 * Phdr
			-nostartfiles -nodefaultlibs -nolibc -nostdlib -static
			-Wl,--omagic
		)
		target_link_options(ray PRIVATE -Wl,-Map,ray.map)
		target_link_options(ray-single PRIVATE -Wl,-Map,ray-single.map)
		if(DO_STRIP)
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
