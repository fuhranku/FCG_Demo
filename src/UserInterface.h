#pragma once

#include "AntTweakBar.h"
#include "glm/glm.hpp"
#include <string>
#include "Model.h"
#include "Light.h"

typedef enum {INT_DIR_LIGHT, INT_FLASHLIGHT} INTERFACE_TYPE;

//Singleton user interface class

class UI{
private:
	static UI * interface; //Holds the instance of the class
	TwBar *tbInterface;

public:
	///Method to obtain the only instance of the calls
	static UI * Instance();
	void reshape();
	//Model attribute:
	std::string modelName;
	glm::vec3 translation;
	float cutOff, outerCutOff,scale;
	glm::vec4 quat;
	void swaptoDirectionalLightMode();
	void swapToPointLightMode();
	void swaptoFlashlightMode();
	void swaptoRegularMode(MATERIAL_TYPE mat_type);
	//Light properties
	Material material;
	glm::vec3 direction;
	glm::vec3 position;
	bool lightSwitch;
	bool cameraMode;
	Attenuation attenuation;
	void updateFromUI(int pickedIndex, std::vector<Model *> *model, std::vector<Light *> *light, bool *cameraMode);
	void setUI(int pickedIndex, std::vector<Model *> model, std::vector<Light *> light, bool cameraMode);
	//----
private:
	///Private constructor
	UI();
	
};