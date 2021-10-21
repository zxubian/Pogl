#include "../include/Render.h"

#include <glm/gtc/type_ptr.hpp>

#include "../include/Shaders.h"
#include "../include/Texture.h"


void render
(
	const Program_render_data* program_render_data,
	const Per_frame_render_data* per_frame_render_data,
	const Things_to_render* things_to_render
)
{
	const glm::mat4x4& view_matrix = per_frame_render_data->view_matrix;
	const glm::mat4x4& projection_matrix = per_frame_render_data->projection_matrix;
	const glm::mat4x4 vp_matrix = projection_matrix * view_matrix;
	const glm::mat4x4 camera_transform = glm::inverse(view_matrix);
	const glm::vec3 camera_world_pos = camera_transform[3];

	// render vertex colors
	const Vertex_col_data vertex_col = things_to_render->vertex_col_data;
	unsigned int distinct_mesh_count = vertex_col.distinct_mesh_count;
	const GLint* uniforms = program_render_data->vertex_col_uniforms;
	const GLint* attributes = program_render_data->vertex_col_attributes;

	glUseProgram(program_render_data->vertex_col_shader_id);
	glUniformMatrix4fv(uniforms[0], 1, GL_FALSE, glm::value_ptr(vp_matrix));
	for(unsigned int mesh_index = 0; mesh_index < distinct_mesh_count; ++mesh_index)
	{
		const Mesh_render_data mesh_data = vertex_col.meshes[mesh_index];
		Mesh* mesh = mesh_data.mesh;
		glBindVertexArray(mesh->vao);
		mesh->positions_attribute_pointer(attributes[0]);
		mesh->colors_attribute_pointer(attributes[1]);
		glEnableVertexAttribArray(attributes[0]);
		glEnableVertexAttribArray(attributes[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
		for(unsigned int instance_index = 0; instance_index < mesh_data.instance_count; ++instance_index)
		{
			const Transform transform = mesh_data.transforms[instance_index];
			glm::mat4x4 model_matrix = transform.object_to_world;
			glUniformMatrix4fv(uniforms[1], 1, GL_FALSE, glm::value_ptr(model_matrix));
			glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, nullptr);
		}
		glDisableVertexAttribArray(attributes[0]);
		glDisableVertexAttribArray(attributes[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// render specular diffuse
	const Specular_diffuse_data& specular_diffuse = things_to_render->specular_diffuse_data;
	distinct_mesh_count = specular_diffuse.distinct_mesh_count;
	uniforms = program_render_data->specular_diffuse_uniforms;
	attributes = program_render_data->specular_diffuse_attributes;
	const Light_data& light_data = per_frame_render_data->light_data;
	glUseProgram(program_render_data->specular_diffuse_shader_id);
	glUniformMatrix4fv(uniforms[1], 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(uniforms[3], 1, GL_FALSE, glm::value_ptr(projection_matrix));
	const glm::vec4& ambient_color = light_data.ambient_color;
	glUniform4f(uniforms[5], ambient_color.x, ambient_color.y, ambient_color.z, ambient_color.w);
	const Directional_light& directional_light = light_data.directional_light;
	const glm::vec4& light_color = directional_light.color;
	glUniform4f(uniforms[6], light_color.x, light_color.y, light_color.z, light_color.w);
	const glm::vec3& light_direction = directional_light.direction;
	glUniform3f(uniforms[8], light_direction.x, light_direction.y, light_direction.z);
	glUniform3f(uniforms[9], camera_world_pos.x, camera_world_pos.y, camera_world_pos.z);
	for(unsigned int mesh_index = 0; mesh_index < distinct_mesh_count; ++mesh_index)
	{
		const Mesh_render_data& mesh_data = specular_diffuse.meshes[mesh_index];
		Mesh* mesh = mesh_data.mesh;
		const Specular_diffuse_instance_render_data* per_instance_data_array = specular_diffuse.per_instance_data[mesh_index];
		glBindVertexArray(mesh->vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
		mesh->positions_attribute_pointer(attributes[0]);
		mesh->texcoords_attribute_pointer(attributes[1]);
		mesh->normals_attribute_pointer(attributes[2]);
		glEnableVertexAttribArray(attributes[0]);
		glEnableVertexAttribArray(attributes[1]);
		glEnableVertexAttribArray(attributes[2]);
		for(unsigned int instance_index = 0; instance_index < mesh_data.instance_count; ++instance_index)
		{
			const Specular_diffuse_instance_render_data& per_instance_data = per_instance_data_array[instance_index];
			const glm::vec4& diffuse_color = per_instance_data.diffuse_color;
			const Transform transform = mesh_data.transforms[instance_index];
			glm::mat4x4 model_matrix = transform.object_to_world;
			glm::mat4x4 model_inv_trans = glm::transpose(glm::inverse(model_matrix));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, per_instance_data.texture->id);
			glUniformMatrix4fv(uniforms[0], 1, GL_FALSE, glm::value_ptr(model_matrix));
			glUniformMatrix4fv(uniforms[2], 1, GL_FALSE, glm::value_ptr(model_inv_trans));
			const glm::vec3& specular_color = per_instance_data.specular_color;
			glUniform3f(uniforms[7], specular_color.x, specular_color.y, specular_color.z);
			glUniform4f(uniforms[4], diffuse_color.x, diffuse_color.y, diffuse_color.z, diffuse_color.w);
			const GLfloat& specular_power = per_instance_data.specular_power;
			glUniform1f(uniforms[10], specular_power);
			glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, nullptr);
		}
		glDisableVertexAttribArray(attributes[0]);
		glDisableVertexAttribArray(attributes[1]);
		glDisableVertexAttribArray(attributes[2]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
}
