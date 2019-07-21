#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vPos;
// Atributte 1 of the vertex
layout (location = 1) in vec2 uvPos;
// Attribute 2 of the vertex
layout (location = 2) in vec3 vNormal;

// Vertex data out data
out vec2 texCoords;
out vec3 normal;
out vec3 fragPos;
// Uniforms
// -----------------------
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    normal = mat3(transpose(inverse(model))) * vNormal;
    texCoords = uvPos; 
    fragPos = vec3 (model * vec4(vPos,1.0));
    gl_Position = projection * view * vec4(fragPos,1.0);
}