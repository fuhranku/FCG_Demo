#version 330 core
// Vertex color (interpolated/fragment)
in vec2 texCoords;

// Fragment Color
out vec3 fragColor;

struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform sampler2D myTexture;


void main(){
    fragColor = material.diffuse ;
}