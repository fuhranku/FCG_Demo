#version 330 core
#define PI 3.14159625

out vec4 FragColor;
  
uniform sampler2D diffMap;
uniform sampler2D specMap;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;
uniform vec3 viewPos;

in VS_OUT{
	vec2 TexCoords;
	vec3 FragPos;
	vec4 FragPosLightSpace;
	vec3 normal;
}fs_in;

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
float shadowMapping(vec4 FragPosLightSpace, vec3 normal, vec3 lightDir);



void main(){
	if (texture(diffMap, fs_in.TexCoords).a == 0.0)
		discard;

	vec3 norm = normalize(fs_in.normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 result = vec3(0.0,0.0,0.0);
	
	// Shadow mapping calculation
	float shadow = shadowMapping(fs_in.FragPosLightSpace, fs_in.normal, dirLight.direction);

    // Directional lighting
	result += calcDirLight(dirLight, norm, viewDir, fs_in.TexCoords, dirLight.direction, shadow) * dirLight.lightSwitch;

    // Point lights
    for (int i = 0;i < 4; i++)
		result += calcPointLight(pointlight[i], norm, fs_in.FragPos, viewDir, fs_in.TexCoords, pointlight[i].position) * pointlight[i].lightSwitch;
    
	// Spot light
	result += calcSpotLight(spotlight,norm,fs_in.FragPos,viewDir, fs_in.TexCoords, spotlight.position, spotlight.direction) * spotlight.lightSwitch;

    //Final fragment
    FragColor = vec4(result,1.0);
}

vec3 calcDirLight(DirLight light,vec3 normal, vec3 viewDir, vec2 texCoords, vec3 LightDirection, float shadow){
    // Getting light direction vector (Only direction since this is a directional only light
    vec3 lightDir = normalize(light.direction);
	
	// Diffuse lighting technique
    float diff = calcDiffLighting(normal,lightDir,viewDir);
	
	// Specular lighting technique
    float spec = calcSpecLighting(normal,lightDir,viewDir);

	// Ambient lighting
	vec3 ambient  = material.ambient * light.ambient;
	
	// Diffuse lighting
	vec3 diffuse  = material.diffuse * light.diffuse * diff;

    // Specular lighting
	vec3 specular =  material.specular * light.specular * spec * texture(specMap, fs_in.TexCoords).rgb;
   
	// Result	
	return (0.8 * ambient + (1.0 - shadow) * (diffuse + specular)) * texture(diffMap, texCoords).rgb;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords, vec3 LightPosition){
	// Getting direction vector between light source and fragment's position
    vec3 lightDir = normalize(light.position - fragPos);
	
	// Attenuation
    float distance    = length(light.position - fragPos);
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
	vec3 specular =  material.specular * light.specular * attenuation * spec * texture(specMap, fs_in.TexCoords).rgb;
   
	// Result	
	return (ambient + diffuse + specular) * texture(diffMap, fs_in.TexCoords).rgb;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords, vec3 LightPosition, vec3 LightDirection){
	// Getting direction vector between light source and fragment's position
	vec3 lightDir = normalize(light.position - fragPos);

	// Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.attenuation.constant + light.attenuation.linear * distance + light.attenuation.quadratic * (distance * distance)); 

	// Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
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
	vec3 specular =  material.specular * light.specular * spec * texture(specMap, fs_in.TexCoords).rgb;
   
	// Result	
	return (0.2f * ambient + diffuse + specular) * texture(diffMap, fs_in.TexCoords).rgb;
}

float calcDiffLighting(vec3 normal,vec3 lightDir, vec3 viewDir){
	// Oren-Nayar technique
	float sigma2 = material.roughness * material.roughness;
	float albedo = 0.7;
	float A = 1.0 - 0.5 * (sigma2 / (sigma2 + 0.57));
	float B = 0.45 * (sigma2 / (sigma2 + 0.09));
	float NdotL = clamp(dot(normal,lightDir),0.0,1.0);
	float NdotV = clamp(dot(normal,viewDir),0.0,1.0);
	float alpha = max(acos(NdotL), acos(NdotV));
	float betha = min(acos(NdotL), acos(NdotV));

	vec3 termA = normalize( (viewDir  - normal) *  NdotV);
	vec3 termB = normalize( (lightDir - normal) *  NdotL);

	float operation = dot(termA,termB);

	return NdotL*(A + max(0, operation) * B * sin(alpha) * tan(betha));
}

float calcSpecLighting(vec3 normal,vec3 lightDir,vec3 viewDir){
	// No specular contribution for this technique
	return 0.0;
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