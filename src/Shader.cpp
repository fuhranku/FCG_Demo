#include "Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char *vertexPath, const char *fragmentPath){
	unsigned vertexID, fragmentID;

	if (!compileShaderCode(vertexPath, shaderType::VERTEX_SHADER, vertexID))
		return;

	if (!compileShaderCode(fragmentPath, shaderType::FRAGMENT_SHADER, fragmentID)){
		glDeleteShader(vertexID);
		return;
	}

	linkProgram(vertexID, fragmentID);

	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
}

Shader::Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath){
	unsigned vertexID, fragmentID, geometryID;

	if (!compileShaderCode(vertexPath, shaderType::VERTEX_SHADER, vertexID))
		return;

	if (!compileShaderCode(fragmentPath, shaderType::FRAGMENT_SHADER, fragmentID)){
		glDeleteShader(vertexID);
		return;
	}

	if (!compileShaderCode(geometryPath, shaderType::GEOMETRY_SHADER, geometryID)){
		glDeleteShader(vertexID);
		glDeleteShader(fragmentID);
		return;
	}

	linkProgram(vertexID, fragmentID, geometryID);

	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
	glDeleteShader(geometryID);
}

Shader::~Shader(){
	glDeleteProgram(ID);
}

void Shader::use(){
	glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string &name, float x, float y) const{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w){
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

bool Shader::compileShaderCode(const char *path, shaderType type, unsigned int &shaderID){
	std::string shaderCode;
	std::ifstream shaderFile;

	// Set exceptions for ifstream object
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try{
		// Open the file
		shaderFile.open(path);
		std::stringstream shaderStream;
		// Reads the buffer content into streams
		shaderStream << shaderFile.rdbuf();
		// Close the file handler
		shaderFile.close();
		// Convert the stream into a string
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e){
		std::cout << "ERROR::SHADER Error reading file: " << path << std::endl;
		return false;
	}

	const char *code = shaderCode.c_str();
	std::string stringType;
	// Creates the shader object in the GPU
	switch (type){
	case VERTEX_SHADER:
		shaderID = glCreateShader(GL_VERTEX_SHADER);
		stringType = "VERTEX";
		break;
	case FRAGMENT_SHADER:
		shaderID = glCreateShader(GL_FRAGMENT_SHADER);
		stringType = "FRAGMENT";
		break;
	case GEOMETRY_SHADER:
		shaderID = glCreateShader(GL_GEOMETRY_SHADER);
		stringType = "GEOMETRY";
		break;
	}
	// Loads the shader code to the GPU
	glShaderSource(shaderID, 1, &code, NULL);
	// Compiles the shader
	glCompileShader(shaderID);

	int succes;
	char log[1024];
	// Get compilation status
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &succes);
	// Compilation error
	if (!succes){
		// Gets the error message
		glGetShaderInfoLog(shaderID, 1024, NULL, log);
		std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << stringType << "\n"
				  << "path: "
				  << path
				  << "\n " << log << "\n -- --------------------------------------------------- -- " << std::endl;
		return false;
	}

	return true;
}

bool Shader::linkProgram(unsigned int vertexShaderID, unsigned int fragmentShaderID){
	// Creates GPU shader program
	ID = glCreateProgram();
	// Attach the vertex shader for linking
	glAttachShader(ID, vertexShaderID);
	// Attach the fragment shader for linking
	glAttachShader(ID, fragmentShaderID);
	// Link the shaders
	glLinkProgram(ID);

	int success;
	char log[1024];
	// Get compilation status
	glGetShaderiv(ID, GL_LINK_STATUS, &success);
	// Compilation error
	if (!success){
		// Gets the error message
		glGetShaderInfoLog(ID, 1024, NULL, log);
		std::cout << "ERROR::PROGRAM_LINKING_ERROR\n"
				  << log << "\n -- --------------------------------------------------- -- " << std::endl;
		return false;
	}

	// Delete all the shaders because they are no longer neccesary
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return true;
}

bool Shader::linkProgram(unsigned int vertexShaderID, unsigned int fragmentShaderID, unsigned int geometryShaderID){
	// Creates GPU shader program
	ID = glCreateProgram();
	// Attach the vertex shader for linking
	glAttachShader(ID, vertexShaderID);
	// Attach the fragment shader for linking
	glAttachShader(ID, fragmentShaderID);
	// Attach the geometry shader for linking
	glAttachShader(ID, geometryShaderID);
	// Link the shaders
	glLinkProgram(ID);

	int success;
	char log[1024];
	// Get compilation status
	glGetShaderiv(ID, GL_LINK_STATUS, &success);
	// Compilation error
	if (!success){
		// Gets the error message
		glGetShaderInfoLog(ID, 1024, NULL, log);
		std::cout << "ERROR::PROGRAM_LINKING_ERROR\n"
				  << log << "\n -- --------------------------------------------------- -- " << std::endl;
		return false;
	}

	// Delete all the shaders because they are no longer neccesary
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	glDeleteShader(geometryShaderID);

	return true;
}

void Shader::setDirectionalLight(Shader *shader, Light dir_light) {
	// Directional light switch
	shader->setVec3("dirLight.direction", dir_light.direction);
	shader->setVec3("dirLight.ambient", dir_light.material.ambient);
	shader->setVec3("dirLight.diffuse", dir_light.material.diffuse);
	shader->setVec3("dirLight.specular", dir_light.material.specular);
	shader->setBool("dirLight.lightSwitch", dir_light.lightSwitch);
	shader->setVec3("light_positions.DirLight_direction", dir_light.direction);
}

void Shader::setPointLight(Shader *shader, Light pointlight, int index) {
	std::string str = std::to_string(index);
	shader->setVec3("pointlight[" + str + "].position", pointlight.position);
	shader->setVec3("pointlight[" + str + "].ambient", pointlight.material.ambient);
	shader->setVec3("pointlight[" + str + "].diffuse", pointlight.material.diffuse);
	shader->setVec3("pointlight[" + str + "].specular", pointlight.material.specular);
	shader->setFloat("pointlight[" + str + "].attenuation.constant", pointlight.attenuation.constant);
	shader->setFloat("pointlight[" + str + "].attenuation.linear", pointlight.attenuation.linear);
	shader->setFloat("pointlight[" + str +"].attenuation.quadratic", pointlight.attenuation.quadratic);
	shader->setBool("pointlight[" + str + "].lightSwitch", pointlight.lightSwitch);
	shader->setVec3("light_positions.PointLight_position[" + str + "]", pointlight.position);
}

void Shader::setSpotLight(Shader *shader, Light *spotlight, Camera camera){
	//Update spotlight position with camera's movement
	spotlight->position = camera.position;
	spotlight->direction = camera.viewDirection;
	
	shader->setVec3("spotlight.position", (*spotlight).position);
	shader->setVec3("spotlight.direction", (*spotlight).direction);
	shader->setVec3("spotlight.ambient", (*spotlight).material.ambient);
	shader->setVec3("spotlight.diffuse", (*spotlight).material.diffuse);
	shader->setVec3("spotlight.specular", (*spotlight).material.specular);
	shader->setFloat("spotlight.cutOff", (*spotlight).cutOff);
	shader->setFloat("spotlight.outerCutOff", (*spotlight).outerCutOff);
	shader->setFloat("spotlight.attenuation.constant", (*spotlight).attenuation.constant);
	shader->setFloat("spotlight.attenuation.linear", (*spotlight).attenuation.linear);
	shader->setFloat("spotlight.attenuation.quadratic", (*spotlight).attenuation.quadratic);
	shader->setBool("spotlight.lightSwitch", (*spotlight).lightSwitch);
	shader->setVec3("light_positions.SpotLight_direction", (*spotlight).direction);
	shader->setVec3("light_positions.SpotLight_position", (*spotlight).position);
}

void Shader::setMVPUniforms(Shader *shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
	shader->setMat4("model", model);
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);
}

void Shader::setModelMaterials(Shader *shader, Material material) {
	shader->setFloat("material.shininess", material.shininess);
	shader->setFloat("material.roughness", material.roughness);
	shader->setFloat("material.height_scale", material.height_scale);
	shader->setInt("material.material_type", material.material_type);
	shader->setInt("material.diffuse_type", material.diffuse_type);
	shader->setInt("material.norm_type", material.norm_type);
	shader->setInt("material.spec_type", material.spec_type);
	shader->setInt("material.depth_type", material.depth_type);
	shader->setVec3("material.ambient", material.ambient);
	shader->setVec3("material.diffuse", material.diffuse);
	shader->setVec3("material.specular", material.specular);
}

void Shader::configModelShader(Shader *shader, Model *model, std::vector<Light *> light, glm::mat4 modelM,
	glm::mat4 view, glm::mat4 projection, glm::mat4 LightProjectionMatrix, Camera camera, int num_of_pointlights) {
	shader->use();
	shader->setInt("diffMap", 0);
	shader->setInt("specMap", 1);
	shader->setInt("normalMap", 2);
	shader->setInt("depthMap", 3);
	shader->setInt("shadowMap", 4);
	shader->setVec3("viewPos", camera.position);
	shader->setMat4("LightProjectionMatrix", LightProjectionMatrix);
	std::string str;
	// Inject MVP matrices into shader
	Shader::setMVPUniforms(shader, modelM, view, projection);
	// Inject Model's materials into shader
	Shader::setModelMaterials(shader, model->material);
	// LIGHTS
		// Inject directional light properties into shader
		Shader::setDirectionalLight(shader, *light[0]);
		// Inject spotlight properties into shader
		Shader::setSpotLight(shader, light[1], camera);
		// Inject point light properties of n lights into shader
		for (int i = 2; i < num_of_pointlights + 2; i++)
			Shader::setPointLight(shader, *light[i], i - 2);
}