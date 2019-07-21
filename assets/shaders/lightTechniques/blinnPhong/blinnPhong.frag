#version 330 core
#define PI 3.14159625

out vec4 FragColor;
  
uniform sampler2D diffMap;
uniform sampler2D specMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform sampler2D shadowMap;


in VS_OUT{
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
} fs_in;

// Fragment properties of all the model
struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
	float roughness;
	// 0: Blinn-phong, 1: Oren_Nayar, 2: Cook_torrance
	int material_type;
	// 0: Albedo 1:color
	int diffuse_type;
	// 0: off 1: specular map
	int spec_type;
	int norm_type;
	float height_scale;
};


// Directional light source properties
struct DirLight{    
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	int lightSwitch;
};

// Point light source properties
struct Attenuation{
    float constant;
    float linear;
    float quadratic;
};

struct PointLight{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    Attenuation attenuation;
	int lightSwitch;
};

// Spot light source properties
struct SpotLight{
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;
	Attenuation attenuation;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	int lightSwitch;
};

// Uniforms
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointlight[4];
uniform SpotLight spotlight;

// Light Casters calculations
vec3 calcDirLight(DirLight light,vec3 normal, vec3 viewDir, vec2 texCoords, vec3 LightDirection, float shadow);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords, vec3 LightPosition); 
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords, vec3 LightPosition, vec3 LightDirection);

// Illumination  Techniques functions
float calcDiffLighting(vec3 normal,vec3 lightDir, vec3 viewDir);
float calcSpecLighting(vec3 normal,vec3 lightDir,vec3 viewDir);
vec2 parallaxMapping(vec2 texCoords, vec3 viewDir);
float shadowMapping(vec4 FragPosLightSpace, vec3 normal, vec3 lightDir);

void main(){

    vec3 result = vec3(0.0,0.0,0.0);

	// View direction in tangent space
    vec3 viewDir = normalize(fs_in.tangViewPos - fs_in.tangFragPos);	
	
	if (texture(diffMap, fs_in.TexCoords).a == 0.0)
		discard;

	// Parallax Mapping
	vec2 texCoords = parallaxMapping(fs_in.TexCoords, viewDir);

	// Normal Mapping
	vec3 norm = texture(normalMap, texCoords).rgb;
	norm = normalize(norm * 2.0f - 1.0f); 
    
	// Shadow mapping calculation
	float shadow = shadowMapping(fs_in.FragPosLightSpace, fs_in.normal, dirLight.direction);

	// Directional lighting
	result += calcDirLight(dirLight, norm, viewDir, texCoords, fs_in.tangDirLight_direction, shadow) * dirLight.lightSwitch;

    // Point lights
    for (int i = 0;i < 4; i++)
		result += calcPointLight(pointlight[i], norm, fs_in.tangFragPos, viewDir, texCoords,fs_in.tangPointLight_position[i]) * pointlight[i].lightSwitch;
	// Spot light
		result += calcSpotLight(spotlight,norm,fs_in.tangFragPos,viewDir, texCoords, fs_in.tangSpotLight_position, fs_in.tangSpotLight_direction) * spotlight.lightSwitch;

    //Final fragment
    FragColor = vec4(result,1.0);
}

vec3 calcDirLight(DirLight light,vec3 normal, vec3 viewDir, vec2 texCoords, vec3 LightDirection, float shadow){
    // Getting light direction vector (Only direction since this is a directional only light
    vec3 lightDir = normalize(LightDirection);
	
	// Diffuse lighting technique
    float diff = calcDiffLighting(normal,lightDir,viewDir);
	
	// Specular lighting technique
    float spec = calcSpecLighting(normal,lightDir,viewDir);

	// Ambient lighting
	vec3 ambient  = material.ambient * light.ambient;
	
	// Diffuse lighting
	vec3 diffuse  = material.diffuse * light.diffuse * diff;

    // Specular lighting
	vec3 specular =  material.specular * light.specular * spec * texture(specMap, texCoords).rgb;
   
	// Result	
	return (0.8 * ambient + (1.0 - shadow) * (diffuse + specular)) * texture(diffMap, texCoords).rgb;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords, vec3 LightPosition){
	// Getting direction vector between light source and fragment's position
    vec3 lightDir = normalize(LightPosition - fragPos);
	
	// Attenuation
    float distance    = length(LightPosition - fragPos);
    float attenuation = 1.0 / (light.attenuation.constant + light.attenuation.linear * distance + 
                        light.attenuation.quadratic * (distance * distance)); 
    // Diffuse lighting technique
    float diff = calcDiffLighting(normal,lightDir,viewDir);
    
	// Specular lighting technique
    float spec =  calcSpecLighting(normal,lightDir,viewDir);

	// Ambient lighting
    vec3 ambient  = material.ambient * light.ambient * attenuation;
	
	// Diffuse lighting
	vec3 diffuse  = material.diffuse * light.diffuse * diff * attenuation;

    // Specular lighting
	vec3 specular =   material.specular * light.specular * attenuation * spec * texture(specMap, texCoords).rgb;
   
	// Result	
	return (0.3 * ambient + diffuse + specular) * texture(diffMap, texCoords).rgb;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords, vec3 LightPosition, vec3 LightDirection){
	// Getting direction vector between light source and fragment's position
	vec3 lightDir = normalize(LightPosition - fragPos);

	// Attenuation
    float distance = length(LightPosition - fragPos);
    float attenuation = 1.0 / (light.attenuation.constant + light.attenuation.linear * distance + light.attenuation.quadratic * (distance * distance)); 

	// Spotlight intensity
    float theta = dot(lightDir, normalize(-LightDirection)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // Diffuse lighting technique
    float diff = calcDiffLighting(normal,lightDir,viewDir);

    // Specular lighting technique
    float spec = calcSpecLighting(normal,lightDir,viewDir);
   
	// Ambient lighting
    vec3 ambient =  material.ambient * light.ambient * attenuation * intensity;
	
	// Diffuse lighting
	vec3 diffuse =  material.diffuse * light.diffuse * attenuation * intensity * diff ;	
    
	// Specular lighting
	vec3 specular =  material.specular * light.specular * spec * attenuation * intensity * texture(specMap, texCoords).rgb;
   
	// Result	
	return (0.3 * ambient + diffuse + specular) * texture(diffMap, texCoords).rgb;
}

float calcDiffLighting(vec3 normal,vec3 lightDir, vec3 viewDir){
	float NdotL = clamp(dot(normal,lightDir),0.0f,1.0f);	 
	// Blinn-Phong Diffuse contribution
	return clamp(NdotL,0.0f,1.0f);
}

float calcSpecLighting(vec3 normal,vec3 lightDir,vec3 viewDir){
	vec3 reflectDir = reflect(-lightDir, normal);
	// Blinn-Phong Specular contribution
	return pow(clamp(dot(viewDir,reflectDir),0.0f,1.0f),material.shininess);
}

vec2 parallaxMapping(vec2 texCoords, vec3 viewDir){
	const float minLayers = 8.0;
	const float maxLayers = 32.0;
	// Num of layers based on view position
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
	// Size of each layer;
	float layerDepth = 1.0 / numLayers;
	// Depth of current layer
	float currentLayerDepth = 0.0f;
	// The amount to shift texture coordinate per layer (from vector P)
	vec2 P = viewDir.xy * material.height_scale;
	vec2 deltaTexCoords = P / numLayers;

	vec2 currentTexCoords = texCoords;
	float currentDepthMapValue = texture(depthMap, currentTexCoords).r;

	while(currentLayerDepth < currentDepthMapValue){
		// Shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// Get depthMap value at current texture coordinates
		currentDepthMapValue = texture(depthMap, currentTexCoords).r;
		// Get depth of next layer
		currentLayerDepth += layerDepth;
	}

	// get texture coordinates before collision (reverse operations)
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;  

}

float shadowMapping(vec4 FragPosLightSpace, vec3 normal, vec3 lightDir){
	 // perform perspective divide
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.005); 
    // check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;

	if (projCoords.z > 1.0)	
        shadow = 0.0;
    return shadow;
}