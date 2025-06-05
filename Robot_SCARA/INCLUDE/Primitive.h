#pragma once
#include "Model.h"
#include <glm.hpp>
#include <memory>
#include "Shader.h"

class Primitive {
public:
	Primitive(const std::string& path, const glm::vec3& position);
	void Draw(const glm::mat4& mvp, const glm::mat4& model, Shader* shader);
	void SetPosition(const glm::vec3& pos);
	glm::vec3 GetPosition() const;


private:
	std::unique_ptr<Model>model;
	glm::vec3 position;
};