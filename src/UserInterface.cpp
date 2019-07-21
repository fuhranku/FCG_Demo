#include "UserInterface.h"


// Global static pointer used to ensure a single instance of the class.
UI * UI::interface = NULL;
// External values of windows size:
extern unsigned int windowWidth, windowHeight;

/**
* Creates an instance of the class
*
* @return the instance of this class
*/
UI * UI::Instance(){
	if (!interface)   // Only allow one instance of class to be generated.
		interface = new UI();

	return interface;
}

UI::UI(){
	tbInterface = TwNewBar("Tarea 1 - FCG");
	
	//Basic config:
	TwDefine("'Tarea 1 - FCG' refresh = '0.0001f'");
	TwDefine("'Tarea 1 - FCG' resizable = false");
	TwDefine("'Tarea 1 - FCG' fontresizable = false");
	TwDefine("'Tarea 1 - FCG' movable = true");
	TwDefine("'Tarea 1 - FCG' position = '10 20'");
	TwDefine("'Tarea 1 - FCG' size = '250 400'");
	TwDefine("'Tarea 1 - FCG' color = '101 57 245'");
	TwDefine("'Tarea 1 - FCG' valueswidth=120 ");
	TwDefine("'Tarea 1 - FCG' visible=false ");


	//Type:	
	TwEnumVal materialType[] = { {BLINN_PHONG, "Blinn-Phong"}, {OREN_NAYAR, "Oren-Nayar"}, {COOK_TORRANCE, "Cook-Torrance"},
								 {REFLECTION,"Reflection"}, {BLEND,"Blending"} };

	TwType DeployMaterialType = TwDefineEnum("Material type", materialType, 5);

	
	// Diff type:
	TwEnumVal diffType[] = { {ALBEDO, "Albedo"}, {COLOR, "Material color"} };
	TwType DeployDiffType = TwDefineEnum("Diffuse Type", diffType, 2);

	// Switch type:
	TwEnumVal switchType[] = {{ false, "OFF" }, { true,"ON" }};
	TwType DeploySwitchType = TwDefineEnum("ON/OFF", switchType, 2);

	//Swap to camera mode
	TwAddVarRW(tbInterface, "Camera mode", DeploySwitchType, &cameraMode, "label='Camera Mode'");

	//Model's name:
	TwAddVarRO(tbInterface, "Model Selected", TW_TYPE_STDSTRING, &modelName, "label='Name' readonly=true");
	TwAddSeparator(tbInterface, "", NULL);

	//Turn light on/off
	TwAddVarRW(tbInterface, "Switch", DeploySwitchType, &lightSwitch, "visible=false");

	// Material type 
	TwAddVarRW(tbInterface, "Material type", DeployMaterialType, &material.material_type, "visible=false label='Shader'");

	// Diffuse type
	TwAddVarRW(tbInterface, "Diffuse type", DeployDiffType, &material.diffuse_type, "visible=false label='Diffuse map'");
	TwAddVarRW(tbInterface, "Specular map", DeploySwitchType, &material.spec_type, "visible=false");
	TwAddVarRW(tbInterface, "Normal map", DeploySwitchType, &material.norm_type, "visible=false");
	TwAddVarRW(tbInterface, "Depth map", DeploySwitchType, &material.depth_type, "visible=false");

	//Model's Attributes
	//Direction 
	TwAddVarRW(tbInterface, "Direction", TW_TYPE_DIR3F, &direction, " axisz=z ");
	//Position 
	TwAddVarRW(tbInterface, "X", TW_TYPE_FLOAT, &position[0], " group='Position' step=0.01");
	TwAddVarRW(tbInterface, "Y", TW_TYPE_FLOAT, &position[1], " group='Position' step=0.01");
	TwAddVarRW(tbInterface, "Z", TW_TYPE_FLOAT, &position[2], " group='Position' step=0.01");
	TwDefine("'Tarea 1 - FCG'/'Position' visible = false");
	// Scale
	TwAddVarRW(tbInterface, "Scale", TW_TYPE_FLOAT, &scale, " visible=false step=0.001");
	// Rotation
	TwAddVarRW(tbInterface, "Rotation", TW_TYPE_QUAT4F, &quat, "visible=false");
	TwAddSeparator(tbInterface, "sep1", NULL);
	
	//Color
	TwAddVarRW(tbInterface, "Ambient", TW_TYPE_COLOR3F, &material.ambient, "group='Material color' label = 'Ambient'");
	TwAddVarRW(tbInterface, "Diffuse", TW_TYPE_COLOR3F, &material.diffuse, "group='Material color' label = 'Diffuse'");
	TwAddVarRW(tbInterface, "Specular", TW_TYPE_COLOR3F, &material.specular, "group='Material color' label = 'Specular'");
	TwAddSeparator(tbInterface, "sep2", NULL);

	//Model attributes

	TwAddVarRW(tbInterface, "Height scale", TW_TYPE_FLOAT, &material.height_scale, "min=0.0 max=0.7 step=0.001 visible = false");
	TwAddVarRW(tbInterface, "Shininess", TW_TYPE_FLOAT, &material.shininess, "min=1.0 max=255 step=0.1 visible = false");
	TwAddVarRW(tbInterface, "Roughness", TW_TYPE_FLOAT, &material.roughness, "min=0.1 max=1.0 step=0.001 visible = false");
	TwAddVarRW(tbInterface, "Reflection", TW_TYPE_FLOAT, &material.reflection, "min=0.0 max=1.0 step=0.001 visible = false");
	
	TwAddVarRW(tbInterface, "RefAmbient", TW_TYPE_FLOAT, &material.ambient_reflective_index, "group='Refractance' label='Ambient' min=1.0 max=3.0 step=0.001");
	TwAddVarRW(tbInterface, "RefMaterial", TW_TYPE_FLOAT, &material.material_reflective_index, "group='Refractance' label='Material' min=1.0 max=3.0 step=0.001");
	TwDefine("'Tarea 1 - FCG'/'Refractance' visible = false");

	//Light attributes
		//Atenuation's type:
		TwAddVarRW(tbInterface, "Constant ", TW_TYPE_FLOAT, &attenuation.constant, "group='Atenuation' min=0.0  step=0.001");
		TwAddVarRW(tbInterface, "Linear   ", TW_TYPE_FLOAT, &attenuation.linear, "group='Atenuation'   min=0.0  step=0.001");
		TwAddVarRW(tbInterface, "Quadratic", TW_TYPE_FLOAT, &attenuation.quadratic, "group='Atenuation' min=0.0 step=0.001");
		//Cutoffs
		TwAddVarRW(tbInterface, "Cutoff", TW_TYPE_FLOAT, &cutOff, "group='Cutoffs' step=0.0001 min=0.0 max=1.0");
		TwAddVarRW(tbInterface, "Outer cutoff", TW_TYPE_FLOAT, &outerCutOff, "group='Cutoffs' step=0.0001 min=0.1 max =0.99");
		TwDefine("'Tarea 1 - FCG'/'Cutoffs' visible = false");

}

void UI::reshape(){
	TwWindowSize((int)windowWidth, (int)windowHeight);
}

void UI::swapToPointLightMode() {
	TwDefine("'Tarea 1 - FCG' visible=true ");
	//Turn other atributes OFF
	TwDefine("'Tarea 1 - FCG'/'Direction' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Rotation' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Scale' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Cutoffs' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Material type' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Diffuse type' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Specular map' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Depth map' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Normal map' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Height scale' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Shininess' visible=false");
	TwDefine("'Tarea 1 - FCG'/'Roughness' visible=false");
	TwDefine("'Tarea 1 - FCG'/'Reflection' visible=false");
	TwDefine("'Tarea 1 - FCG'/'Refractance' visible=false");

	//Turn Point light attributes ON:
	TwDefine("'Tarea 1 - FCG'/'Position' visible = true");
	TwDefine("'Tarea 1 - FCG'/'Atenuation' visible = true");
	TwDefine("'Tarea 1 - FCG'/'Switch' visible = true");
	TwDefine("'Tarea 1 - FCG'/'Diffuse' visible=true");

}

void UI::swaptoDirectionalLightMode() {
	TwDefine("'Tarea 1 - FCG' visible=true ");
	//Turn other atributes OFF
	TwDefine("'Tarea 1 - FCG'/'Atenuation' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Scale' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Rotation' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Position' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Cutoffs' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Material type' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Diffuse type' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Specular map' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Normal map' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Depth map' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Height scale' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Shininess' visible=false");
	TwDefine("'Tarea 1 - FCG'/'Reflection' visible=false");
	TwDefine("'Tarea 1 - FCG'/'Refractance' visible=false");

	//Turn Directional light attributes ON:
	TwDefine("'Tarea 1 - FCG'/'Direction' visible = true");
	TwDefine("'Tarea 1 - FCG'/'Switch' visible = true");
	TwDefine("'Tarea 1 - FCG'/'Diffuse' visible=true");
}

void UI::swaptoFlashlightMode() {
	TwDefine("'Tarea 1 - FCG' visible=true ");
	//Turn other atributes OFF
	TwDefine("'Tarea 1 - FCG'/'Direction' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Scale' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Position' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Rotation' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Shininess' visible=false");
	TwDefine("'Tarea 1 - FCG'/'Roughness' visible=false");
	TwDefine("'Tarea 1 - FCG'/'Material type' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Diffuse type' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Normal map' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Specular map' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Depth map' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Height scale' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Reflection' visible=false");
	TwDefine("'Tarea 1 - FCG'/'Refractance' visible=false");

	//Turn flashlight atributes ON
	TwDefine("'Tarea 1 - FCG'/'Cutoffs' visible = true");
	TwDefine("'Tarea 1 - FCG'/'Atenuation' visible = true");
	TwDefine("'Tarea 1 - FCG'/'Switch' visible = true");
	TwDefine("'Tarea 1 - FCG'/'Diffuse' visible=true");
}

void UI::swaptoRegularMode(MATERIAL_TYPE mat_type) {
	TwDefine("'Tarea 1 - FCG' visible=true ");
	//Turn other atributes OFF
	TwDefine("'Tarea 1 - FCG'/'sep1' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Atenuation' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Cutoffs' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Direction' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Switch' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Shininess' visible=false");
	TwDefine("'Tarea 1 - FCG'/'Roughness' visible=false");
	TwDefine("'Tarea 1 - FCG'/'Diffuse' visible=true");

	//Turn regular model attributes ON:
	TwDefine("'Tarea 1 - FCG'/'Position' visible = true");
	TwDefine("'Tarea 1 - FCG'/'Scale' visible = true");
	TwDefine("'Tarea 1 - FCG'/'Rotation' visible = true");
	TwDefine("'Tarea 1 - FCG'/'Material type' visible = true");
	TwDefine("'Tarea 1 - FCG'/'Diffuse type' visible = true");
	TwDefine("'Tarea 1 - FCG'/'Specular map' visible = true");
	TwDefine("'Tarea 1 - FCG'/'Normal map' visible = true");
	TwDefine("'Tarea 1 - FCG'/'Depth map' visible = true");
	TwDefine("'Tarea 1 - FCG'/'Height scale' visible = false");
	TwDefine("'Tarea 1 - FCG'/'Shininess' visible=false");
	TwDefine("'Tarea 1 - FCG'/'Diffuse' visible=false");
	TwDefine("'Tarea 1 - FCG'/'Reflection' visible=false");
	TwDefine("'Tarea 1 - FCG'/'Refractance' visible=false");
	TwDefine("'Tarea 1 - FCG'/'Roughness' visible=false");

	if (mat_type == BLINN_PHONG) {
		TwDefine("'Tarea 1 - FCG'/'Shininess' visible=true");
		TwDefine("'Tarea 1 - FCG'/'Diffuse' visible=true");
		TwDefine("'Tarea 1 - FCG'/'Height scale' visible = true");
	}
	else if (mat_type == OREN_NAYAR){
		TwDefine("'Tarea 1 - FCG'/'Diffuse' visible=false");
		TwDefine("'Tarea 1 - FCG'/'Roughness' visible=true min=0.0");
	}
	else if (mat_type == REFLECTION) {
		TwDefine("'Tarea 1 - FCG'/'Shininess' visible=true");
		TwDefine("'Tarea 1 - FCG'/'Diffuse' visible=true");
		TwDefine("'Tarea 1 - FCG'/'Reflection' visible=true");
		TwDefine("'Tarea 1 - FCG'/'Refractance' visible=true");
	}
	else {
		TwDefine("'Tarea 1 - FCG'/'Roughness' visible=true min=0.01");
		TwDefine("'Tarea 1 - FCG'/'Diffuse' visible=true");
	}

}

void UI::updateFromUI(int pickedIndex, std::vector<Model *> *model, std::vector<Light *> *light, bool *cameraMode){
	*cameraMode = this->cameraMode;
	// If there's no object selected
	if (pickedIndex == INT_MIN)
		return;

	switch (pickedIndex) {
	// Directional light interface
	case -2:
		// Set parameters into tweakbar
		(*light)[0]->direction = this->direction;
		(*light)[0]->material.ambient = this->material.ambient;
		(*light)[0]->material.diffuse = this->material.diffuse;
		(*light)[0]->material.specular = this->material.specular;
		(*light)[0]->lightSwitch = this->lightSwitch;
		break;
	// Flashlight
	case -1:
		(*light)[1]->cutOff = this->cutOff;
		(*light)[1]->outerCutOff = this->outerCutOff;
		(*light)[1]->material.ambient = this->material.ambient;
		(*light)[1]->material.diffuse = this->material.diffuse;
		(*light)[1]->material.specular = this->material.specular;
		(*light)[1]->attenuation.constant = this->attenuation.constant;
		(*light)[1]->attenuation.linear = this->attenuation.linear;
		(*light)[1]->attenuation.quadratic = this->attenuation.quadratic;
		(*light)[1]->lightSwitch = this->lightSwitch;
		break;
	// Pointlight interface
	case 0: case 1: case 2: case 3:
		//Set parameters into tweakbar
		(*light)[(*model)[pickedIndex]->lightIndex]->position = this->position;
		(*light)[(*model)[pickedIndex]->lightIndex]->material.ambient = this->material.ambient;
		(*light)[(*model)[pickedIndex]->lightIndex]->material.diffuse = this->material.diffuse;
		(*light)[(*model)[pickedIndex]->lightIndex]->material.specular = this->material.specular;
		(*light)[(*model)[pickedIndex]->lightIndex]->attenuation.constant = this->attenuation.constant;
		(*light)[(*model)[pickedIndex]->lightIndex]->attenuation.linear = this->attenuation.linear;
		(*light)[(*model)[pickedIndex]->lightIndex]->attenuation.quadratic = this->attenuation.quadratic;
		(*light)[(*model)[pickedIndex]->lightIndex]->lightSwitch = this->lightSwitch;
		break;
	// Regular model interface
	default:
		(*model)[pickedIndex]->material.ambient = this->material.ambient;
		(*model)[pickedIndex]->material.diffuse = this->material.diffuse;
		(*model)[pickedIndex]->material.specular = this->material.specular;
		(*model)[pickedIndex]->material.shininess = this->material.shininess;
		(*model)[pickedIndex]->material.reflection = this->material.reflection;
		(*model)[pickedIndex]->material.roughness = this->material.roughness;
		(*model)[pickedIndex]->material.height_scale = this->material.height_scale;
		(*model)[pickedIndex]->material.material_type = this->material.material_type;
		(*model)[pickedIndex]->material.diffuse_type = this->material.diffuse_type;
		(*model)[pickedIndex]->material.spec_type = this->material.spec_type;
		(*model)[pickedIndex]->material.norm_type = this->material.norm_type;
		(*model)[pickedIndex]->material.depth_type = this->material.depth_type;
		(*model)[pickedIndex]->material.ambient_reflective_index = this->material.ambient_reflective_index;
		(*model)[pickedIndex]->material.material_reflective_index = this->material.material_reflective_index;
		(*model)[pickedIndex]->transformations.translate = this->position;
		(*model)[pickedIndex]->transformations.scale = this->scale;
		(*model)[pickedIndex]->transformations.quat = this->quat;
		break;
	}
}

void UI::setUI(int pickedIndex, std::vector<Model *> model, std::vector<Light *> light, bool cameraMode) {
	this->cameraMode = cameraMode;
	// If there's no object selected
	if (pickedIndex == INT_MIN)
		return;

	switch (pickedIndex){
	// Directional light
	case -2:
		// Set parameters into tweakbar
		this->modelName = light[0]->name;
		this->direction = light[0]->direction;
		this->material.ambient = light[0]->material.ambient;
		this->material.diffuse = light[0]->material.diffuse;
		this->material.specular = light[0]->material.specular;
		this->lightSwitch = light[0]->lightSwitch;
		// Swap to appropiate interface
		this->swaptoDirectionalLightMode();
		break;
	// Flashlight
	case -1:
		// Set parameters into tweakbar
		this->modelName = light[1]->name;
		this->cutOff = light[1]->cutOff;
		this->outerCutOff = light[1]->outerCutOff;
		this->material.ambient = light[1]->material.ambient;
		this->material.diffuse = light[1]->material.diffuse;
		this->material.specular = light[1]->material.specular;
		this->attenuation.constant = light[1]->attenuation.constant;
		this->attenuation.linear = light[1]->attenuation.linear;
		this->attenuation.quadratic = light[1]->attenuation.quadratic;
		this->lightSwitch = light[1]->lightSwitch;
		// Swap to appropiate interface
		this->swaptoFlashlightMode();
		break;
	// Pointlight
	case 0: case 1: case 2: case 3: 
		//Set parameters into tweakbar
		this->modelName = light[model[pickedIndex]->lightIndex]->name;
		this->position = light[model[pickedIndex]->lightIndex]->position;
		this->material.ambient = light[model[pickedIndex]->lightIndex]->material.ambient;
		this->material.diffuse = light[model[pickedIndex]->lightIndex]->material.diffuse;
		this->material.specular = light[model[pickedIndex]->lightIndex]->material.specular;
		this->attenuation.constant = light[model[pickedIndex]->lightIndex]->attenuation.constant;
		this->attenuation.linear = light[model[pickedIndex]->lightIndex]->attenuation.linear;
		this->attenuation.quadratic = light[model[pickedIndex]->lightIndex]->attenuation.quadratic;
		this->lightSwitch = light[model[pickedIndex]->lightIndex]->lightSwitch;
		
		// Swap to appropiate interface
		this->swapToPointLightMode();
		break;
	// Regular model interface
	default:
		this->modelName = model[pickedIndex]->name;
		this->material.ambient = model[pickedIndex]->material.ambient;
		this->material.diffuse = model[pickedIndex]->material.diffuse;
		this->material.specular = model[pickedIndex]->material.specular;
		this->material.shininess = model[pickedIndex]->material.shininess;
		this->material.reflection = model[pickedIndex]->material.reflection;
		this->material.roughness = model[pickedIndex]->material.roughness;
		this->material.height_scale = model[pickedIndex]->material.height_scale;
		this->material.material_type = model[pickedIndex]->material.material_type;
		this->material.diffuse_type = model[pickedIndex]->material.diffuse_type;
		this->material.spec_type = model[pickedIndex]->material.spec_type;
		this->material.norm_type = model[pickedIndex]->material.norm_type;
		this->material.depth_type = model[pickedIndex]->material.depth_type;
		this->material.ambient_reflective_index = model[pickedIndex]->material.ambient_reflective_index;
		this->material.material_reflective_index = model[pickedIndex]->material.material_reflective_index;
		this->position = model[pickedIndex]->transformations.translate;
		this->scale = model[pickedIndex]->transformations.scale;
		this->quat = model[pickedIndex]->transformations.quat;

		// Swap to appropiate interface
		this->swaptoRegularMode(model[pickedIndex]->material.material_type);
	}
}