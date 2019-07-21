#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 uvPos;

// Uniforms
// -----------------------
uniform mat4 model;
uniform mat4 LightProjectionMatrix;

out vec2 TexCoords;

void main()
{
	TexCoords = uvPos;
    gl_Position = LightProjectionMatrix * model * vec4(vPos,1.0f);
}