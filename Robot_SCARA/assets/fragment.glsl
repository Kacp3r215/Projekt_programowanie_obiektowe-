#version 330 core
in vec3 FragNormal;
uniform vec3 lightDir;
uniform vec3 lightColor;
out vec4 FragColor;

void main() {
    vec3 norm = normalize(FragNormal);
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 baseColor = vec3(0.5, 0.5, 0.5);
    vec3 result = baseColor * diffuse;
    FragColor = vec4(result, 1.0);
}
