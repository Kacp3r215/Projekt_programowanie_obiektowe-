#pragma once

#include <vector>
#include <string>
#include <map>
#include <assimp/scene.h>
#include "Mesh.h"

using namespace std;

class Model {
public:
	map<string, Mesh> meshes;

	Model(const string& path);
	void Draw() const;
	Mesh* getMesh(const string& name);

private:
	void loadModel(const string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};
