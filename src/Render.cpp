#include "../include/Render.h"

#include <glm/gtc/type_ptr.hpp>

#include "../include/Texture.h"

constexpr GLuint MAX_POINT_LIGHTS = 10;

void set_directional_light(const GLuint& id, const Directional_light& data)
{
	const glm::vec4& color = data.color;
	glUniform4f(glGetUniformLocation(id, "directional_light.color"), color.x, color.y, color.z, color.w);
	const glm::vec3& direction = data.direction;
	glUniform3f(glGetUniformLocation(id, "directional_light.direction"), direction.x, direction.y, direction.z);
}

void set_point_lights(const GLuint& id, const Point_light* point_lights, GLuint point_light_count)
{
	char location_string[128]{'\0'};
	point_light_count = glm::min(point_light_count, MAX_POINT_LIGHTS);
	glUniform1i(glGetUniformLocation(id, "point_light_count"), static_cast<int>(point_light_count));
	for(GLuint i = 0; i < point_light_count; ++i)
	{
		const Point_light light = point_lights[i];
		const glm::vec3 attenuation_constants = calculate_attenuation_constant(light.max_distance, light.falloff);
		const glm::vec3 color = light.color;
		const glm::vec3 world_position = light.world_position;
		sprintf_s(location_string, "point_lights[%d].color", i);
		glUniform3f(glGetUniformLocation(id, location_string), color.x, color.y, color.z);
		sprintf_s(location_string, "point_lights[%d].position", i);
		glUniform3f(glGetUniformLocation(id, location_string), world_position.x, world_position.y, world_position.z);
		sprintf_s(location_string, "point_lights[%d].attenuation_constants", i);
		glUniform3f(glGetUniformLocation(id, location_string), attenuation_constants.x, attenuation_constants.y, attenuation_constants.z);
	}
}

void set_instance_data(const GLuint id, const Specular_diffuse_instance_render_data& data)
{
	const glm::vec4& diffuse_color = data.diffuse_color;
	glUniform4f(glGetUniformLocation(id, "per_instance.diffuse_color"), diffuse_color.x, diffuse_color.y, diffuse_color.z, diffuse_color.w);
	const glm::vec3& specular_color = data.specular_color;
	glUniform3f(glGetUniformLocation(id, "per_instance.specular_color"), specular_color.x, specular_color.y, specular_color.z);
	const glm::vec3& ambient_color = data.ambient_color;
	glUniform3f(glGetUniformLocation(id, "per_instance.ambient_color"), ambient_color.x, ambient_color.y, ambient_color.z);
	const glm::vec4& intensities = data.intensities;
	glUniform4f(glGetUniformLocation(id, "per_instance.intensities"), intensities.x, intensities.y, intensities.z, intensities.w);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(id, "per_instance.albedo_tex"), 0);
	glBindTexture(GL_TEXTURE_2D, data.albedo_texture->id);
}

void render
(
	const Program_render_data* program_render_data,
	const Per_frame_render_data* per_frame_render_data,
	const Things_to_render* things_to_render
)
{
	//TODO: @performance cache uniform location once per shader

	const glm::mat4x4& view_matrix = per_frame_render_data->view_matrix;
	const glm::mat4x4& projection_matrix = per_frame_render_data->projection_matrix;
	const glm::mat4x4 vp_matrix = projection_matrix * view_matrix;
	const glm::mat4x4 camera_transform = glm::inverse(view_matrix);
	const glm::vec3 camera_world_pos = camera_transform[3];

	// render vertex colors
	GLuint shader_id = program_render_data->vertex_col_shader_id;
	const Vertex_col_data vertex_col = things_to_render->vertex_col_data;
	unsigned int distinct_mesh_count = vertex_col.distinct_mesh_count;

	glUseProgram(shader_id);
	glUniformMatrix4fv(glGetUniformLocation(shader_id, "vp_matrix"), 1, GL_FALSE, glm::value_ptr(vp_matrix));
	for(unsigned int mesh_index = 0; mesh_index < distinct_mesh_count; ++mesh_index)
	{
		const Mesh_render_data& mesh_data = vertex_col.meshes[mesh_index];
		const Mesh* mesh = mesh_data.mesh;
		glBindVertexArray(mesh->vao);
		const GLuint pos_location = glGetAttribLocation(shader_id, "pos");
		const GLuint col_location = glGetAttribLocation(shader_id, "col");
		mesh->positions_attribute_pointer(pos_location);
		mesh->colors_attribute_pointer(col_location);
		glEnableVertexAttribArray(pos_location);
		glEnableVertexAttribArray(col_location);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
		for(unsigned int instance_index = 0; instance_index < mesh_data.instance_count; ++instance_index)
		{
			const Transform transform = mesh_data.transforms[instance_index];
			glm::mat4x4 model_matrix = transform.object_to_world;
			glUniformMatrix4fv(glGetUniformLocation(shader_id, "model_matrix"), 1, GL_FALSE, glm::value_ptr(model_matrix));
			glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, nullptr);
		}
		glDisableVertexAttribArray(pos_location);
		glDisableVertexAttribArray(col_location);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// render specular diffuse
	const Specular_diffuse_data& specular_diffuse = things_to_render->specular_diffuse_data;

	shader_id = program_render_data->specular_diffuse_shader_id;

	distinct_mesh_count = specular_diffuse.distinct_mesh_count;

	glUseProgram(shader_id);

	glUniformMatrix4fv(glGetUniformLocation(shader_id, "view_matrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(glGetUniformLocation(shader_id, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));

	glUniform3f(glGetUniformLocation(shader_id, "camera_world_pos"), camera_world_pos.x, camera_world_pos.y, camera_world_pos.z);

	const Light_data& light_data = per_frame_render_data->light_data;

	set_directional_light(shader_id, light_data.directional_light);

	set_point_lights(shader_id, light_data.point_lights, light_data.point_light_count);

	for(unsigned int mesh_index = 0; mesh_index < distinct_mesh_count; ++mesh_index)
	{
		const Mesh_render_data& mesh_data = specular_diffuse.meshes[mesh_index];
		Mesh* mesh = mesh_data.mesh;
		const Specular_diffuse_instance_render_data* per_instance_data_array = specular_diffuse.per_instance_data[mesh_index];
		glBindVertexArray(mesh->vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);

		GLuint pos_location = glGetAttribLocation(shader_id, "pos");
		GLuint uv_location = glGetAttribLocation(shader_id, "uv");
		GLuint norm_location = glGetAttribLocation(shader_id, "norm");
		mesh->positions_attribute_pointer(pos_location);
		mesh->texcoords_attribute_pointer(uv_location);
		mesh->normals_attribute_pointer(norm_location);
		glEnableVertexAttribArray(pos_location);
		glEnableVertexAttribArray(uv_location);
		glEnableVertexAttribArray(norm_location);
		for(unsigned int instance_index = 0; instance_index < mesh_data.instance_count; ++instance_index)
		{
			const Transform transform = mesh_data.transforms[instance_index];
			glm::mat4x4 model_matrix = transform.object_to_world;
			glm::mat4x4 model_inv_trans_matrix = glm::transpose(glm::inverse(model_matrix));

			glUniformMatrix4fv(glGetUniformLocation(shader_id, "model_matrix"), 1, GL_FALSE, glm::value_ptr(model_matrix));
			glUniformMatrix4fv(glGetUniformLocation(shader_id,"model_inv_trans_matrix"), 1,GL_FALSE, glm::value_ptr(model_inv_trans_matrix));

			const Specular_diffuse_instance_render_data& per_instance_data = per_instance_data_array[instance_index];

			set_instance_data(shader_id, per_instance_data);

			glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, nullptr);
		}
		glDisableVertexAttribArray(pos_location);
		glDisableVertexAttribArray(uv_location);
		glDisableVertexAttribArray(norm_location);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
}
