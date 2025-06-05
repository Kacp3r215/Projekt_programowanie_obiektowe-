#include "Primitive.h"

Primitive::Primitive(const std::string& path, const glm::vec3& position)
	: model(std::make_unique<Model>(path)), position(position){}

void Primitive::Draw(const glm::mat4& mvp, const glm::mat4& modelMat, Shader* shader) {
    shader->use();
    shader->setMat4("mvp", mvp);
    shader->setMat4("model", modelMat);
    model->Draw();
}

void Primitive::SetPosition(const glm::vec3& pos) {
    position = pos;
}

glm::vec3 Primitive::GetPosition() const {
    return position;
}