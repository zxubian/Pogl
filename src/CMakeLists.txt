file(GLOB SOURCE_LIST *.cpp)
file(GLOB HEADER_LIST ../include *.h)
file(GLOB SHADERS "../data/shaders/*.vert" "../data/shaders/*.frag" )
source_group("Shaders" FILES ${SHADERS})

# Add source to this project's executable.
add_executable (${PROJECT_NAME} ${SOURCE_LIST} ${HEADER_LIST} ${SHADERS} )

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED) # First try to find using the find_package method (works with vcpkg)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLFW3_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE glfw OpenGL::GL ${GLEW_LIBRARIES} glm::glm)
