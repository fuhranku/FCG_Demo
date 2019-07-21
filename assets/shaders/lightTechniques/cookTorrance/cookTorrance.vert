#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vPos;
// Atributte 1 of the vertex
layout (location = 1) in vec2 uvPos;
// Attribute 2 of the vertex
layout (location = 2) in vec3 vNormal;

// Vertex data out data
out VS_OUT{
	vec2 TexCoords;
	vec3 FragPos;
	vec4 FragPosLightSpace;
	vec3 normal;
}vs_out;

// Uniforms
// -----------------------
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 LightProjectionMatrix;

void main()
{
    vs_out.normal = mat3(transpose(inverse(model))) * vNormal;
    vs_out.TexCoords = uvPos; 
    vs_out.FragPos = vec3(model * vec4(vPos,1.0));
	vs_out.FragPosLightSpace = LightProjectionMatrix * vec4(vs_out.FragPos, 1.0f);
    gl_Position = projection * view * vec4(vs_out.FragPos,1.0);
}