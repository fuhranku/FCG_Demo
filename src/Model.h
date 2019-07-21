#pragma once
#include <glad/glad.h> // Glad has to be included before glfw
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include "glm/glm.hpp"

typedef enum { NEW_OBJECT,VERTICES,VTEXTURE,VNORMAL,FACES,NONE} DATA_TYPE;
typedef enum { MODEL, POINTLIGHT} MODEL_TYPE;
typedef enum { BLINN_PHONG, OREN_NAYAR, COOK_TORRANCE, REFLECTION, BLEND}MATERIAL_TYPE;
typedef enum { ALBEDO, COLOR}DIFFUSE_TYPE;

struct Data {
	glm::vec3 vertex;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};

struct Material {
	MATERIAL_TYPE material_type;
	DIFFUSE_TYPE diffuse_type;
	float shininess;
	float roughness;
	float height_scale;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	int spec_type;
	int norm_type;
	int depth_type;
	float reflection;
	float material_reflective_index;
	float ambient_reflective_index;
};

struct Transformations {
	glm::vec3 rotate;
	glm::vec3 translate;
	glm::vec4 quat;
	float scale;
	float angle;
};

class Model {
public:
	Model();
	unsigned int dataSize, VBO, VAO;
	MODEL_TYPE type;
	char name[255];
	float pickColor[3];
	bool isEmpty;
	int lightIndex;
	unsigned int albedo;
	unsigned int specMap;
	unsigned int normalMap;
	unsigned int depthMap;
	Material material;
	Transformations transformations;
	// Load aux variables
	std::vector<glm::vec3> vertices, normals, tangent, bitangent;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> vIndices, uvIndices, nIndices;
	std::vector<Data> verticesData;

	// Functions
	static bool loadModel(std::string path,std::vector<Model *> *model, float* pickColor,MODEL_TYPE type);
	static glm::mat4 toMat4(glm::vec4 quat);
	static unsigned int loadCubemap(std::vector<std::string> faces);
	static void sortWindows(std::vector<Model *> *model, glm::vec3 cameraPos);
private:
	static DATA_TYPE getToken(std::string token);
};