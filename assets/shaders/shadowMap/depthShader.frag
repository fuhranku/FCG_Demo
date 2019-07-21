#version 330 core

in vec2 TexCoords;

uniform sampler2D diffMap;

void main()
{             
	if (texture(diffMap,TexCoords).a < 0.01f)
		discard;
    // gl_FragDepth = gl_FragCoord.z;
}  