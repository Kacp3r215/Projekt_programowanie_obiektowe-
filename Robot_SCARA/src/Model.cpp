#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

using namespace std;

Model::Model(const string& path) {
	loadModel(path);
}

Mesh* Model::getMesh(const string& name) {
	auto it = meshes.find(name);
	if (it != meshes.end()) {
		return &it->second;
	}
	return nullptr;
}


void Model::loadModel(const string& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		cout << "Assimp error: " << importer.GetErrorString() << endl;
		return ;
	}

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh processedMesh = processMesh(mesh, scene);
		meshes[processedMesh.name] = processedMesh;
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* /*scene*/) {
	vector<Vertex> vertices;
	vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		vertex.position = glm::vec3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		);

		vertex.normal = mesh->HasNormals() ? glm::vec3(
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		) : glm::vec3(0.0f);

		vertex.texCoords = mesh->mTextureCoords[0] ?
			glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) :
			glm::vec2(0.0f);

		vertices.push_back(vertex);
	}
	
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	string name = mesh->mName.C_Str();
	std::cout << "Loaded mesh: " << name << std::endl; // Debugowanie nazw
	return Mesh(name, vertices, indices);
}

void Model::Draw() const {
	for (const auto& pair : meshes)
		pair.second.Draw();
}