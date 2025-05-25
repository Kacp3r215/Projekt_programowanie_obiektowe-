#pragma once

#include <vector>
#include <string>
#include <assimp/scene.h>
#include "Mesh.h"

using namespace std;

class Model {
public:
	vector<Mesh> meshes;

	Model(const string& path);
	void Draw() const;

private:
	void loadModel(const string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};
