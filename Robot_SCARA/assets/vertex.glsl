#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
out vec3 FragNormal;
uniform mat4 mvp;
uniform mat4 model;

void main() {
    FragNormal = mat3(transpose(inverse(model))) * normal;
    gl_Position = mvp * vec4(position, 1.0);
}
