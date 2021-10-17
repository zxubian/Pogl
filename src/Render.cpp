#include "../include/Render.h"

#include <glm/gtc/type_ptr.hpp>

#include "../include/Shaders.h"


void render
(
	const Program_render_data* program_render_data, 
	const Per_frame_render_data* per_frame_render_data, 
	const Things_to_render* things_to_render
)
{
	// render vertex colors
	const Vertex_col_data shader_data = things_to_render->vertex_col_data;
	const unsigned int distinct_mesh_count = shader_data.distinct_mesh_count;
	GLint* uniforms = program_render_data->vertex_col_uniforms;
	const glm::mat4x4 view_matrix = per_frame_render_data->view_matrix;
	const glm::mat4x4 projection_matrix = per_frame_render_data->projection_matrix;
	const glm::mat4x4 vp_matrix = projection_matrix * view_matrix;
	for(unsigned int mesh_index = 0; mesh_index < distinct_mesh_count; ++mesh_index)
	{
		const Mesh_render_data mesh_data = shader_data.meshes[mesh_index];
		glUniformMatrix4fv(uniforms[0], 1, GL_FALSE, glm::value_ptr(vp_matrix));
		Mesh* mesh = mesh_data.mesh;
			glBindVertexArray(mesh-);
		for(unsigned int instance_index = 0; instance_index < mesh_data.instance_count; ++instance_index)
		{
			const Transform transform = mesh_data.transforms[instance_index];
			glm::mat4x4 model_matrix = transform.object_to_world;
			glUniformMatrix4fv(uniforms[1], 1, GL_FALSE, glm::value_ptr(model_matrix));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
	}
}
