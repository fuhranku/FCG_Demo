#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vPos;
// Atributte 1 of the vertex
layout (location = 1) in vec2 uvPos;
// Attribute 2 of the vertex
layout (location = 2) in vec3 vNormal;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(vPos, 1.0);
    FragPos = worldPos.xyz; 
    TexCoords = uvPos;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * vNormal;

    gl_Position = projection * view * worldPos;
}