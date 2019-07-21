#include "Model.h"

Model::Model(){
	srand(NULL);
	isEmpty = true;
	material.ambient = glm::vec3(1.0f);
	material.diffuse = glm::vec3(1.0f);
	material.specular = glm::vec3(1.0f);
	material.shininess = 1.0f;
	material.roughness = 0.4f;
	material.height_scale = 0.1f;
	material.material_type = (MATERIAL_TYPE) (rand() % 2);
	material.diffuse_type = (DIFFUSE_TYPE) (rand() % 1);
	material.material_reflective_index = 1.0f;
	material.ambient_reflective_index = 1.0f;
	// Transformations
	transformations.translate = glm::vec3(1.0f);
	transformations.scale = 1.0f;
	transformations.rotate = glm::vec3(1.0f);
	transformations.angle = 1.0f;
	transformations.quat = glm::vec4(1.0f,0.0f,0.0f,0.0f);
}


DATA_TYPE Model::getToken(std::string token) {
	if (token == "o")
		return NEW_OBJECT;
	if (token == "v")
		return VERTICES;
	if (token == "vt")
		return VTEXTURE;
	if (token == "vn")
		return VNORMAL;
	if (token == "f")
		return FACES;
	return NONE;
}

bool Model::loadModel(std::string path, std::vector<Model *> *model, float* pickColor, MODEL_TYPE type) {
	FILE *file;
	fopen_s(&file, path.c_str(), "r");
	char token[255];
	int check;
	//Temporary place for vertices,normals and uv coordinates:
	glm::vec3 vCoords;
	glm::vec3 nCoords;
	glm::vec2 tCoords;
	std::vector<int> AVG;
	glm::ivec3 vIndex, uvIndex, nIndex;
	//Temporary data structures to push all the objects inside this file:
	Model *obj = new Model();
	Data *data1 = new Data();
	Data *data2 = new Data();
	Data *data3 = new Data();

	//Verify if path was valid. If not, return.
	if (file == NULL) {
		std::cout << "Impossible to open the file" << std::endl;
		return false;
	}

	//Main cycle:
	while (true){
		check = fscanf_s(file, "%s", token,sizeof(token));
		//Exit condition:
		if (check == EOF)
			break;
		//Analize token:
		check = Model::getToken(token);
		switch (check) {
		case NEW_OBJECT:
			fscanf_s(file, "%s\n", obj->name, sizeof(obj->name));
			obj->isEmpty = false;
			//printf("Nuevo objeto a procesar: %s\n", obj->name);
			break;
		//Handle vertices line @obj file
		case VERTICES:
			fscanf_s(file, "%f %f %f\n", &vCoords.x, &vCoords.y, &vCoords.z);
			obj->vertices.push_back(vCoords);
			obj->tangent.push_back(glm::vec3(0.0f,0.0f,0.0f));
			obj->bitangent.push_back(glm::vec3(0.0f,0.0f,0.0f));
			AVG.push_back(0);
			break;
		//Handle textures line @obj file
		case VTEXTURE:
			fscanf_s(file, "%f %f\n", &tCoords.x, &tCoords.y);
			obj->uvs.push_back(tCoords);
			break;
		//Handle normals line @obj file
		case VNORMAL:
			fscanf_s(file, "%f %f %f\n", &nCoords.x, &nCoords.y, &nCoords.z);
			obj->normals.push_back(nCoords);
			break;
		//Handle faces line @obj file
		case FACES:
			check = fscanf_s(\
				file, "%i/%i/%i %i/%i/%i %i/%i/%i\n", \
				&vIndex[0], &uvIndex[0], &nIndex[0],
				&vIndex[1], &uvIndex[1], &nIndex[1],
				&vIndex[2], &uvIndex[2], &nIndex[2]
			);
			//This parser only load models with texture coordinates
			if (check != 9) {
				std::cout << "File can't be read by this parser: Try exporting with uv coords." << std::endl;
				return false;
			}
			for (int i = 0; i < 3; i++) {
				obj->vIndices.push_back(vIndex[i]);
				obj->uvIndices.push_back(uvIndex[i]);
				obj->nIndices.push_back(nIndex[i]);
			}
			break;
		}
	}
	//Handle last object of the file:
	//Time to push an object to model's vector
	//printf("Procesando data de %s para guardar | size: %i\n", obj->name, obj->vIndices.size());

	/*
	for (unsigned int i = 0; i < obj->vIndices.size(); i += 3){
		glm::vec3 v1 = obj->vertices[(obj->vIndices[i] - 1)];
		glm::vec3 v2 = obj->vertices[(obj->vIndices[i + 1] - 1)];
		glm::vec3 v3 = obj->vertices[(obj->vIndices[i + 2] - 1)];

		glm::vec2 uv1 = obj->uvs[(obj->uvIndices[i] - 1)];
		glm::vec2 uv2 = obj->uvs[(obj->uvIndices[i + 1] - 1)];
		glm::vec2 uv3 = obj->uvs[(obj->uvIndices[i + 2] - 1)];

		// Compute Tangent and Bitangent
		// Edges of the triangle : position delta
		glm::vec3 edge1 = v2 - v1;
		glm::vec3 edge2 = v3 - v1;

		// UV delta
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

		glm::vec3 tangent = f * (deltaUV2.y  * edge1 - deltaUV1.y * edge2);
		glm::vec3 bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

		// Sum tangent value per triangle
		obj->tangent[(obj->vIndices[i] - 1)] += tangent;
		obj->tangent[(obj->vIndices[i + 1] - 1)] += tangent;
		obj->tangent[(obj->vIndices[i + 2] - 1)] += tangent;

		obj->bitangent[(obj->vIndices[i] - 1)] += bitangent;
		obj->bitangent[(obj->vIndices[i + 1] - 1)] += bitangent;
		obj->bitangent[(obj->vIndices[i + 2] - 1)] += bitangent;

		AVG[(obj->vIndices[i] - 1)]++;
		AVG[(obj->vIndices[i + 1] - 1)]++;
		AVG[(obj->vIndices[i + 2] - 1)]++;
	}

	for (unsigned int i = 0; i < obj->vertices.size(); i++) {
		obj->tangent[i] /= AVG[i];
		obj->bitangent[i] /= AVG[i];
		obj->tangent[i] = glm::normalize(obj->tangent[i]);
		obj->bitangent[i] = glm::normalize(obj->bitangent[i]);
	}

	for (unsigned int i = 0; i < obj->vIndices.size(); i++) {
		//Data structure for all the vertices that makes object's mesh.
		// Vertex i
		data1->vertex    = obj->vertices[(obj->vIndices[i] - 1)];
		data1->normal    = obj->normals[(obj->nIndices[i] - 1)];
		data1->uv        = obj->uvs[(obj->uvIndices[i] - 1)];
		data1->tangent   = obj->tangent[(obj->vIndices[i] - 1)];
		data1->bitangent   = obj->bitangent[(obj->vIndices[i] - 1)];

		//// Vertex i + 1
		//data2->vertex = obj->vertices[(obj->vIndices[i + 1] - 1)];
		//data2->normal = obj->normals[(obj->nIndices[i + 1] - 1)];
		//data2->uv = obj->uvs[(obj->uvIndices[i + 1] - 1)];

		//// Vertex i + 2
		//data3->vertex = obj->vertices[(obj->vIndices[i + 2] - 1)];
		//data3->normal = obj->normals[(obj->nIndices[i + 2] - 1)];
		//data3->uv = obj->uvs[(obj->uvIndices[i + 2] - 1)];
		
		obj->verticesData.push_back(*data1);

		delete data1;
		data1 = new Data();
	}
	*/

	for (unsigned int i = 0; i < obj->vIndices.size(); i++) {
		//Data structure for all the vertices that makes object's mesh.
		data1->vertex = obj->vertices[(obj->vIndices[i] - 1)];
		data1->normal = obj->normals[(obj->nIndices[i] - 1)];
		data1->uv = obj->uvs[(obj->uvIndices[i] - 1)];
		obj->verticesData.push_back(*data1);
		delete data1;
		data1 = new Data();
	}
	 //Compute tangent and bitangent
	glm::vec3 tangent, bitangent;
	for (unsigned int i = 0; i < obj->verticesData.size(); i += 3) {
		// Shortcuts for vertices
		glm::vec3 & v0 = obj->verticesData[i].vertex;
		glm::vec3 & v1 = obj->verticesData[i + 1].vertex;
		glm::vec3 & v2 = obj->verticesData[i + 2].vertex;

		// Shortcuts for UVs
		glm::vec2 & uv0 = obj->verticesData[i].uv;
		glm::vec2 & uv1 = obj->verticesData[i + 1].uv;
		glm::vec2 & uv2 = obj->verticesData[i + 2].uv;

		// Edges of the triangle : postion delta
		glm::vec3 edge1 = v1 - v0;
		glm::vec3 edge2 = v2 - v0;

		// UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		// Finding tangent and bitangent
		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent = glm::normalize(tangent);

		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent = glm::normalize(bitangent);

		// Saving it into model's data
		obj->verticesData[i].tangent = tangent;
		obj->verticesData[i].bitangent = bitangent;

		obj->verticesData[i + 1].tangent = tangent;
		obj->verticesData[i + 1].bitangent = bitangent;

		obj->verticesData[i + 2].tangent = tangent;
		obj->verticesData[i + 2].bitangent = bitangent;
	}

	obj->pickColor[0] = pickColor[0]; obj->pickColor[1] = pickColor[1]; obj->pickColor[2] = pickColor[2];
	pickColor[0] -= 0.01f;
	if (type == MODEL)
		obj->type = type;
	else
		obj->type = POINTLIGHT;
	obj->material.spec_type = 0;
	obj->material.norm_type = 0;
	//Push new object
	model->push_back(obj);
	fclose(file);

	return true;

}

glm::mat4 Model::toMat4(glm::vec4 quat){
	glm::mat4 rotateMatrix(1.0f);
	float xx, xy, xz, xw;
	float yy, yz, yw;
	float zz, zw;
	float ww;
	
	xx = quat.x * quat.x;
	xy = quat.x * quat.y;
	xz = quat.x * quat.z;
	xw = quat.x * quat.w;

	yy = quat.y * quat.y;
	yz = quat.y * quat.z;
	yw = quat.y * quat.w;

	zz = quat.z * quat.z;
	zw = quat.z * quat.w;

	ww = quat.w * quat.w;

	rotateMatrix[0][0] = xx + yy - zz - ww;
	rotateMatrix[0][1] = 2.0f*yz - 2.0f * xw;
	rotateMatrix[0][2] = 2 * yw + 2 * xz;

	rotateMatrix[1][0] = 2.0f * yz + xw;
	rotateMatrix[1][1] = xx - yy + zz - ww;
	rotateMatrix[1][2] = 2.0f * zw - 2.0f * xy;

	rotateMatrix[2][0] = 2.0f * yw - 2.0f * xz;
	rotateMatrix[2][1] = 2.0f * zw + 2.0f * xy;
	rotateMatrix[2][2] = xx - yy - zz + ww;
	
	return rotateMatrix;
}

unsigned int Model::loadCubemap(std::vector<std::string> faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	// Flips the texture when loads it because in opengl the texture coordinates are flipped
	stbi_set_flip_vertically_on_load(false);
	int width, height, nrChannels;
	unsigned char *data;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void Model::sortWindows(std::vector<Model *> *model, glm::vec3 cameraPos) {
	int n = model->size();
	// Order it using bubble sort
	bool swapped;
	float e1, e2;
	for (int i = 50; i < n - 1; i++){
		swapped = false;
		for (int j = 0; j < n - i - 1; j++)
		{
			e1 = glm::length(cameraPos - (*model)[i+j]->transformations.translate);
			e2 = glm::length(cameraPos - (*model)[i+j+1]->transformations.translate);
			if ( e1 < e2){
				std::swap((*model)[i+j], (*model)[i+j+1]);
				swapped = true;
			}
		}

		// IF no two elements were swapped by inner loop, then break 
		if (swapped == false)
			break;
	}
}