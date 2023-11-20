# CMakeList.txt : CMake project for Assignment1c, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.13)

project(Raytracer)

set(SOURCEFILES
	Main.cpp
	LightSource.cpp
	Input_processor.cpp
	Vector3.cpp
	Point3.cpp
	Color.cpp
	Sphere.cpp
	Ray.cpp
	Triangle.cpp
)

set(HEADERFILES
	LightSource.h
	Input_processor.h
	Vector3.h
	Point3.h
	Color.h
	Sphere.h
	Ray.h
	Triangle.h
)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} ${SOURCEFILES} ${HEADERFILES})

# TODO: Add tests and install targets if needed.
set_target_properties(Raytracer PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "../setup.txt")
