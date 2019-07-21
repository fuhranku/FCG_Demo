#version 330 core
// Atributte 0 of the vertex
layout (location = 0) in vec3 vPos;
// Atributte 1 of the vertex
layout (location = 1) in vec2 uvPos;
// Attribute 2 of the vertex
layout (location = 2) in vec3 vNormal;
// Attribute 3 of the vertex
layout (location = 3) in vec3 vTangent;
// Attribute 4 of the vertex
layout (location = 4) in vec3 vBitangent;

// Light positions
struct LightPositions{
	vec3 DirLight_direction;
	vec3 PointLight_position[4];
	vec3 SpotLight_position;
	vec3 SpotLight_direction;
};

// Vertex data out data
out VS_OUT{
	vec2 TexCoords;
	vec3 FragPos;
	vec3 tangFragPos;
	vec3 tangViewPos;
	vec3 tangDirLight_direction;
	vec3 tangPointLight_position[4];
	vec3 tangSpotLight_position;
	vec3 tangSpotLight_direction;
	vec4 FragPosLightSpace;
	vec3 T;
	vec3 B;
	vec3 N;
	vec3 normal;
	mat3 TBN;
} vs_out;

// Uniforms
// -----------------------
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 LightProjectionMatrix;
uniform vec3 viewPos;
uniform LightPositions light_positions;


void main()
{
	vs_out.FragPos = vec3 (model * vec4(vPos,1.0));			
    vs_out.TexCoords = uvPos; 

	vs_out.T = normalize(vec3(model * vec4(vTangent,   0.0f)));
	//vs_out.B = normalize(vec3(model * vec4(vBitangent,   0.0f)));
	vs_out.N = normalize(vec3(model * vec4(vNormal,   0.0f)));
	// re-orthogonalize T with respect to N
	vs_out.T = normalize(vs_out.T - dot(vs_out.T, vs_out.N) * vs_out.N);
	// then retrieve perpendicular vector B with the cross product of T and N
	vs_out.B = cross(vs_out.N, vs_out.T);

    // TBN Matrix
    mat3 TBN = inverse(mat3(vs_out.T,vs_out.B,vs_out.N));
    vs_out.TBN = TBN;
    // Values to tangent space
	vs_out.tangFragPos = TBN * vs_out.FragPos;
	vs_out.tangViewPos = TBN * viewPos;

	// Light's position to tangent space
	vs_out.tangDirLight_direction = TBN * normalize(light_positions.DirLight_direction);
	vs_out.tangSpotLight_position = TBN * light_positions.SpotLight_position;
	vs_out.tangSpotLight_direction = TBN * normalize(light_positions.SpotLight_direction);
	vs_out.FragPosLightSpace = LightProjectionMatrix * vec4(vs_out.FragPos, 1.0f);
	for(int i=0;i<4;i++)
		vs_out.tangPointLight_position[i] = TBN * light_positions.PointLight_position[i];

    gl_Position = projection * view * vec4(vs_out.FragPos,1.0);
}