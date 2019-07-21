#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vPos;
// Atributte 1 of the vertex
layout (location = 1) in vec2 uvPos;

// Vertex data out data
out vec2 texCoords;
// Uniforms
// -----------------------
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    texCoords = uvPos; 
    gl_Position = projection * view * model *vec4(vPos, 1.0f);
}