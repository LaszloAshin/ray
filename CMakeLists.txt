cmake_minimum_required(VERSION 3.0)
project(ray VERSION 0.0.1)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
	add_compile_options(/W4 /WX)
else()
	add_compile_options(-Wall -Wextra -Wpedantic -Werror)

	add_compile_options(-ffunction-sections -fdata-sections)
	add_link_options(-Wl,--gc-sections)
endif()

#add_compile_options(-march=native)
#CXXFLAGS += -fno-exceptions -fno-rtti
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
