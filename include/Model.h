#pragma once


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Texture.h"

class Model
{
public:
	Model(const char* file_path);
	void load_model(const char* file_path);
	Mesh** meshes;
	unsigned int mesh_count;
private:
	void load_node(aiNode* node, const aiScene* scene);
	Mesh* load_mesh(aiMesh* mesh, const aiScene* scene);
	Texture** load_material_textures(aiMaterial* mat, aiTextureType type, unsigned int& count);
};