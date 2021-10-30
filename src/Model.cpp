#include "../include/Model.h"
#include <glm/glm.hpp>
#include <iostream>

Model::Model(const char* file_path)
{
	load_model(file_path);
	mesh_count = 0;
}

void Model::load_model(const char* file_path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals| aiProcess_JoinIdenticalVertices);
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "[assimp]" << importer.GetErrorString() << std::endl;
		return;
	}
	mesh_count = scene->mNumMeshes;
	if(mesh_count > 0)
	{
		meshes = new Mesh*[mesh_count];
	}
	load_node(scene->mRootNode, scene);
}

void Model::load_node(aiNode* node, const aiScene* scene)
{
	for(unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes[i] = load_mesh(mesh, scene);
	}
	for(unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		load_node(node->mChildren[i], scene);
	}
}

Mesh* Model::load_mesh(aiMesh* mesh, const aiScene* scene)
{
	unsigned int vertex_count = mesh->mNumVertices;
	glm::vec3* vertex_positions = new glm::vec3[vertex_count];
	glm::vec3* normals = new glm::vec3[vertex_count];
	glm::vec2* texcoords = new glm::vec2[vertex_count];
	bool has_texcoords = mesh->HasTextureCoords(0);
	for(unsigned int i = 0; i < vertex_count; ++i)
	{
		auto vertex = mesh->mVertices[i];
		vertex_positions[i] = glm::vec3(vertex.x, vertex.y, vertex.z);
		if(has_texcoords)
		{
			auto texcoord = mesh->mTextureCoords[0][i];
			texcoords[i] = glm::vec2(texcoord.x, texcoord.y);
		}
		else
		{
			texcoords[i] = glm::vec2(0);
		}
		auto normal = mesh->mNormals[i];
		normals[i] = glm::vec3(normal.x, normal.y, normal.z);
	}
	unsigned int triangle_count = mesh->mNumFaces;
	unsigned* indices = new unsigned[triangle_count * 3];
	unsigned int index = 0;
	for(unsigned int i = 0; i < triangle_count; ++i)
	{
		auto face = mesh->mFaces[i];
		for(int j = 0; j < 3; ++j)
		{
			indices[index] = face.mIndices[i];
			index +=1 ;
		}
	}
	Texture** diffuse_maps = nullptr;
	unsigned int diffuse_count = 0;
	if(mesh->mMaterialIndex>0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE, diffuse_count);
	}
	Mesh* result = new Mesh();
	result->create_mesh
	(
		reinterpret_cast<GLfloat*>(vertex_positions), reinterpret_cast<GLfloat*>(texcoords),
	                    reinterpret_cast<GLfloat*>(normals), indices, vertex_count, index,
		diffuse_maps, diffuse_count
	);
	return result;
}

Texture** Model::load_material_textures(aiMaterial* mat, aiTextureType type, unsigned int& count)
{
	count = mat->GetTextureCount(type);
	if(count == 0)
	{
		return nullptr;
	}
	Texture** textures = new Texture * [count];
	for(unsigned int i = 0; i < count; ++i)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		Texture* texture = new Texture(str.C_Str());
		texture->load_texture_rgba();
		textures[i] = texture;
	}
	return textures;
}
