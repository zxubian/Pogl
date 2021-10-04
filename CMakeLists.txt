# CMakeList.txt : CMake project for Pogl, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set ( PROJECT_NAME "Pogl")

project( ${PROJECT_NAME} )

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "Pogl.cpp" "Pogl.h")

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED) # First try to find using the find_package method (works with vcpkg)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLFW3_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE glfw OpenGL::GL ${GLEW_LIBRARIES} glm::glm)
