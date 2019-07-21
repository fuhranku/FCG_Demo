#pragma once
#include <glad/glad.h> // Glad has to be included before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include <vector>
#include "glm/glm.hpp"
#include "Camera.h"
#include "Model.h"

typedef enum {CONSTANT, LINEAR, CUADRATIC } ATENUATION_TYPE;

struct Attenuation {
	float constant;
	float linear;
	float quadratic;
};

class Light{
// Variables
public:
	Light();
	~Light();
	std::string name;
	Material material;
	Attenuation attenuation;
	glm::vec3 direction;
	glm::vec3 position;
	bool lightSwitch;
	float cutOff;
	float outerCutOff;
// Functions
	static void initDirectionalLight(std::vector<Light *> *light);
	static void initPointLight(std::vector<Light *> *light, int num_of_light);
	static void initSpotLight(std::vector<Light *> *light, Camera camera);
};
