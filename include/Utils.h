#pragma once

#include <glm/glm.hpp>
#include <gl/glew.h>

constexpr float tau = 6.28318531f;
constexpr float to_radians = tau / 360.f;

void generate_normals(const glm::vec3* positions, const GLuint* indicies, GLuint vertex_count, GLuint index_count, glm::vec3* normals);
