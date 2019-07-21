#include "Light.h"


Light::Light(){
}


Light::~Light(){
}

void Light::initDirectionalLight(std::vector<Light *> *light) {
	Light *dir_light = new Light();

	// Init of directional light's name
	dir_light->name = "Sun";
	// Init direction
	glm::vec3 dir(21.97f, -25.02f, -17.3f);
	dir_light->direction = dir;

	// Init Ambient color
	glm::vec3 ambient(0.3f, 0.3f, 0.3f);
	dir_light->material.ambient = ambient;

	// Init Difusse color
	glm::vec3 diffuse(1.0f, 1.0f, 1.0f);
	dir_light->material.diffuse = diffuse;

	// Init Specular color
	glm::vec3 specular(0.78f, 1.0f, 0.50f);
	dir_light->material.specular = specular;

	// Init light switch's state
	dir_light->lightSwitch = true;

	// Save light into light's vector
	light->push_back(dir_light);
}

void Light::initPointLight(std::vector<Light *> *light, int num_of_light) {
	Light *pointLight = new Light();
	// Init of directional light's name
	pointLight->name = "Light " + std::to_string(num_of_light + 1);

	// Init position
	glm::vec3 pos(1.0f,1.0f,1.0f);
	pointLight->position = pos;
	
	// Init Ambient color
	glm::vec3 ambient(1.0f, 0.0f, 0.0f);
	pointLight->material.ambient = ambient;
	
	// Init Difusse color
	glm::vec3 diffuse(1.0f, 1.0f, 1.0f);
	pointLight->material.diffuse = diffuse;

	// Init Specular light color
	glm::vec3 specular(0.78f, 1.0f, 0.50f);
	pointLight->material.specular = specular;
	
	// Init Atenuations:
	pointLight->attenuation.constant = 1.0f;
	pointLight->attenuation.linear = 0.09f;
	pointLight->attenuation.quadratic = 0.032f;

	// Init light switch's state
	pointLight->lightSwitch = true;

	// Save into light's vector
	light->push_back(pointLight);
}

void Light::initSpotLight(std::vector<Light *> *light, Camera camera) {
	Light *spotlight = new Light();

	// Init directional light's name
	spotlight->name = "Flashlight";

	// Init direction
	spotlight->direction = camera.position;

	// Init position
	spotlight->position = camera.viewDirection;

	// Init Ambient color
	glm::vec3 ambient(0.0f, 1.0f, 0.0f);
	spotlight->material.ambient = ambient;

	// Init Diffuse color
	glm::vec3 diffuse(1.0f, 1.0f, 1.0f);
	spotlight->material.diffuse = diffuse;

	// Specular light color
	glm::vec3 specular(0.78f, 1.0f, 0.50f);
	spotlight->material.specular = specular;

	// Init cutoff
	spotlight->cutOff = glm::cos(glm::radians(12.5f));

	// Init outercutoff
	spotlight->outerCutOff = glm::cos(glm::radians(17.5f));

	// Init attenuation
	spotlight->attenuation.constant = 1.0f;
	spotlight->attenuation.linear = 0.09f;
	spotlight->attenuation.quadratic = 0.032f;

	//Light switch
	spotlight->lightSwitch = false;

	// Save into light's vector
	light->push_back(spotlight);
}