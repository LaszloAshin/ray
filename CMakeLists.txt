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

	add_compile_options(-ffunction-sections -fdata-sections)
	if(APPLE)
		add_link_options(-Wl,-dead_strip)
	else()
		add_link_options(-Wl,--gc-sections -Wl,--as-needed)
	endif()
endif()

add_definitions(-DHAVE_CONFIG_H)
add_definitions(-D_USE_MATH_DEFINES)

find_package(Threads)

add_executable(ray
	src/BaseObject.cc
	src/Color.cc
	src/Image.cc
	src/Light.cc
	src/MultithreadedTracer.cc
	src/Plane.cc
	src/Ray.cc
	src/Scene.cc
	src/Spheroid.cc
	src/Targa.cc
	src/Tracer.cc
	src/Vector.cc
	src/VectorMath.cc
)

target_link_libraries(ray ${CMAKE_THREAD_LIBS_INIT})
