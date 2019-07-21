#version 330 core
#define PI 3.14159625

out vec4 FragColor;
  
uniform sampler2D diffMap;
uniform sampler2D specMap;
uniform sampler2D normalMap;
uniform samplerCube skybox;
uniform vec3 viewPos;
uniform float reflection;

in vec2 texCoords;
in vec3 normal;
in vec3 fragPos;

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
	float ambient_reflective_index;
	float material_reflective_index;
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
vec4 calcDirLight(DirLight light,vec3 normal, vec3 viewDir, vec2 texCoords, vec3 LightDirection);
vec4 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords, vec3 LightPosition); 
vec4 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords, vec3 LightPosition, vec3 LightDirection);

// Illumination  Techniques functions
float calcDiffLighting(vec3 normal,vec3 lightDir, vec3 viewDir);
float calcSpecLighting(vec3 normal,vec3 lightDir,vec3 viewDir);




void main(){

	vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec4 result = vec4(0.0f);
	
	if (texture(diffMap, texCoords).a == 0.0)
		discard;

	// Directional lighting
	//result += calcDirLight(dirLight, norm, viewDir, texCoords, dirLight.direction) * dirLight.lightSwitch;

    // Point lights
//    for (int i = 0;i < 4; i++)
//		result += calcPointLight(pointlight[i], norm, fragPos, viewDir, texCoords,pointlight[i].position) * pointlight[i].lightSwitch;
//	// Spot light
//	result += calcSpotLight(spotlight,norm,fragPos,viewDir, texCoords, spotlight.position, spotlight.direction) * spotlight.lightSwitch;

    //Final fragment
    FragColor = texture(diffMap,texCoords);
}

vec4 calcDirLight(DirLight light,vec3 normal, vec3 viewDir, vec2 texCoords, vec3 LightDirection){
    // Getting light direction vector (Only direction since this is a directional only light
    vec3 lightDir = normalize(-LightDirection);
	
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
	return vec4((ambient + diffuse + specular),1.0f);
}

vec4 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords, vec3 LightPosition){
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
	vec3 specular =  material.specular * light.specular * attenuation * spec * texture(specMap, texCoords).rgb;
   
	// Result	
	return vec4((ambient + diffuse + specular),1.0f);
}

vec4 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords, vec3 LightPosition, vec3 LightDirection){
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
	return vec4((ambient + diffuse + specular),1.0f);
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