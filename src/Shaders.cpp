#include "../include/Shaders.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

inline void get_uniforms_vertex_col(GLuint id, GLint* uniforms)
{
	uniforms[0] = glGetUniformLocation(id, "mvp_matrix");
}

inline void bind_uniforms_vertex_col
(
	const GLint* uniforms,
	const glm::mat4x4& mvp_matrix
)
{
}
