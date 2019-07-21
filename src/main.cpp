#include "main.h"

// Window current width
unsigned int windowWidth = 800;
// Window current height
unsigned int windowHeight = 600;
// Window title
const char *windowTitle = "Tarea 2 - Tecnicas Avanzadas de texturizado y sombras";
// Window pointer
GLFWwindow *window;
//Tweakbar instance:
UI *ui;
// Shader object
Shader *lampShader, *pickingShader, *lightShader, *blinnPhongShader, *orenNayarShader,
	   *cookTorranceShader, *skyboxShader, *reflectionShader, *blendingShader, *depthShader,
	   *depthQuadShader;
// ID of 
GLuint MatrixID, skyboxVAO;
// Model-View-Projection matrix
glm::mat4 View;
glm::mat4 Projection;
glm::mat4 ModelMatrix;
glm::mat4 LightProjectionMatrix;
//Model's vector:
std::vector<Model *> model;
//Light's vector:
std::vector<Light *> light;
//Camera:
Camera camera;
// Textures:
unsigned int texture_white, texture_black, texture_blue, cubemapTexture,
			 depthMapTexture;
// Frame buffers:
unsigned int depthMapFBO;
// Quad for shadow mapping
unsigned int quadVAO = 0;
unsigned int quadVBO;
//Camera mode:
bool cameraMode, cameraView = false;
float pickColor[3] = { 1.0f,0.0f,0.0f };
int pickedIndex = INT_MIN;
int num_of_pointlights;
float deltaTime = 0.0f, lastFrame = 0.0f;

//Callback functions: 

void onResizeWindow(GLFWwindow* window, int width, int height) {
	TwWindowSize(width, height);
}

void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
	TwEventKeyGLFW(key, action);
	//Regular keyboard
	if (action == GLFW_PRESS && !cameraMode) {
		switch (key) {
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, true);
				break;
			case GLFW_KEY_R:
				// Reload shaders
				// Update blinn-phong shader
				delete blinnPhongShader;
				blinnPhongShader =
					new	Shader("assets/shaders/lightTechniques/blinnPhong/blinnPhong.vert",
						"assets/shaders/lightTechniques/blinnPhong/blinnPhong.frag");

				// Update Oren-Nayar shader
				delete orenNayarShader;
				orenNayarShader =
					new	Shader("assets/shaders/lightTechniques/OrenNayar/orenNayar.vert",
						"assets/shaders/lightTechniques/OrenNayar/orenNayar.frag");
				// Update Cook-Torrance shader
				delete cookTorranceShader;
				cookTorranceShader =
					new	Shader("assets/shaders/lightTechniques/cookTorrance/cookTorrance.vert",
						"assets/shaders/lightTechniques/cookTorrance/cookTorrance.frag");
				
				// Update lamp shaders
				delete lampShader;
				lampShader =
					new Shader("assets/shaders/lamp/lamp.vert",
						"assets/shaders/lamp/lamp.frag");
				
				// Update skybox shader
				delete skyboxShader;
				skyboxShader =
					new	Shader("assets/shaders/skyBox/skyBox.vert",
						"assets/shaders/skyBox/skyBox.frag");
				
				// Update reflection and reflection shader
				delete blendingShader;
				blendingShader =
					new	Shader("assets/shaders/blend/blend.vert",
						"assets/shaders/blend/blend.frag");
				
				// Update depth map texture shader
				delete depthShader;
				depthShader =
					new Shader("assets/shaders/shadowMap/depthShader.vert",
						"assets/shaders/shadowMap/depthShader.frag");
				// Update depth quad drawing shader
				delete depthQuadShader;
				depthQuadShader =
					new Shader("assets/shaders/shadowMap/depthQuadShader.vert",
						"assets/shaders/shadowMap/depthQuadShader.frag");
				printf("Shaders succesfully reloaded!\n");
				delete reflectionShader;
				reflectionShader =
					new Shader("assets/shaders/reflection_refraction/reflection.vert",
						"assets/shaders/reflection_refraction/reflection.frag");
				break;
			case GLFW_KEY_F:
				if (mods == 1) {
					if (pickedIndex != -1 ) pickedIndex = INT_MIN;
					printf("Camera mode activated\n");
					cameraMode = true;
				}
				else {
					pickedIndex = -1;
					ui->setUI(pickedIndex, model, light, cameraView);
				}
				break;
			case GLFW_KEY_D:
				pickedIndex = -2;
				// Load info of selected element into tweakbar's interface:
				ui->setUI(pickedIndex, model, light, cameraView);
				break;
		}
	}
	//Camera keyboard
	else if (action == GLFW_PRESS && cameraMode || action == GLFW_REPEAT && cameraMode){
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, true);
			break;
		case GLFW_KEY_F:
			if (mods == GLFW_MOD_SHIFT) {
				printf("Camera mode deactivated\n");
				cameraMode = false;
			}
			break;
		}
	}

}

void processKeyboardInput(GLFWwindow *window){

	// per-frame time logic
	// --------------------
	float currentFrame = (float) glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (cameraMode) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.moveForward(deltaTime);
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.moveLeft(deltaTime);
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.moveBackward(deltaTime);
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.moveRight(deltaTime);
		else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			camera.moveUp(deltaTime);
		else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			camera.moveDown(deltaTime);
	}

}

void onCharacter(GLFWwindow* window, unsigned int codepoint) {
	TwKeyPressed(codepoint, TW_KMOD_NONE);
}

void onMouseButton(GLFWwindow * window, int button, int action, int mods)
{
	auto a = action == GLFW_PRESS ? TW_MOUSE_PRESSED : TW_MOUSE_RELEASED;
	auto b = TW_MOUSE_LEFT;

	TwMouseButton(a, b);
	if (action == TW_MOUSE_PRESSED) {
		if (button == 0 && pickedIndex == INT_MIN)
			pick();
			// Load info of selected element into tweakbar's interface:
			ui->setUI(pickedIndex, model, light, cameraView);
			if (button == 1)
				pickedIndex = INT_MIN;
	}
}

void onMouseMotion(GLFWwindow * window, double xpos, double ypos)
{
	TwMouseMotion(px(static_cast<int>(xpos)), px(static_cast<int>(ypos)));
	glm::vec2 mousePosition(xpos, ypos);
	if (cameraMode)
		camera.mouseUpdate(mousePosition);
}

void resize(GLFWwindow *window, int width, int height) {
	windowWidth = width;
	windowHeight = height;
	// Sets the OpenGL viewport size and position
	glViewport(0, 0, windowWidth, windowHeight);
}

// --------------------------------------------------------------------------------------

//Init Functions:

bool initWindow(){
    // Initialize glfw
    glfwInit();
    // Sets the Opegl context to Opengl 3.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creates the window
    window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
	// Creates the glfwContext, this has to be made before calling initGlad()
	glfwMakeContextCurrent(window);
    // The window couldn't be created
    if (!window){
        std::cout << "Failed to create the glfw window" << std::endl;
        glfwTerminate(); // Stops the glfw program
        return false;
    }

	TwWindowSize(windowWidth, windowHeight);

	glfwSetFramebufferSizeCallback(window, resize);
	glfwSetCursorPosCallback(window, onMouseMotion);
	glfwSetMouseButtonCallback(window, onMouseButton);
	glfwSetKeyCallback(window, onKeyPress);
	glfwSetCharCallback(window, onCharacter);
	glfwSetWindowSizeCallback(window, onResizeWindow);
  
    return true;
}

bool initGlad(){
    // Initialize glad
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    // If something went wrong during the glad initialization
    if (!status){
        std::cout << status << std::endl;
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    return true;
}

bool initATB() {
	if (!TwInit(TW_OPENGL_CORE, NULL))
		return false;
	ui = UI::Instance();

	return true;
}

void initGL(){
    // Enables the z-buffer test
    glEnable(GL_DEPTH_TEST);
	// Enables Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Sets the ViewPort
    glViewport(0, 0, windowWidth, windowHeight);
    // Sets the clear color (Screen color)
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

void configureDepthMap() {
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_BUFFER_SIZE, DEPTH_BUFFER_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f,1.0f,1.0f,1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// Depth frame buffer configuration
	glGenFramebuffers(1, &depthMapFBO);
	// Attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	// Buffer object won't have color or stencil buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool init() {

	// Initialize the window, glad components and user interface
	if (!initWindow() || !initGlad() || !initATB())
		return false;
	// Initialize the opengl context
	initGL();

	// Initialize naive textures
	texture_black = loadTexture("assets/textures/black_bg.png");
	texture_white = loadTexture("assets/textures/white_bg.png");
	texture_blue  = loadTexture("assets/textures/blue_bg.png");

	// Load shaders
	lampShader = new Shader("assets/shaders/lamp/lamp.vert", "assets/shaders/lamp/lamp.frag");
	pickingShader = new Shader("assets/shaders/picking/picking.vert", "assets/shaders/picking/picking.frag");
	blinnPhongShader =
		new	Shader("assets/shaders/lightTechniques/blinnPhong/blinnPhong.vert",
			"assets/shaders/lightTechniques/blinnPhong/blinnPhong.frag");
	orenNayarShader =
		new	Shader("assets/shaders/lightTechniques/OrenNayar/orenNayar.vert",
			"assets/shaders/lightTechniques/OrenNayar/orenNayar.frag");
	cookTorranceShader =
		new	Shader("assets/shaders/lightTechniques/cookTorrance/cookTorrance.vert",
			"assets/shaders/lightTechniques/cookTorrance/cookTorrance.frag");
	skyboxShader =
		new	Shader("assets/shaders/skyBox/skyBox.vert",
			"assets/shaders/skyBox/skyBox.frag");
	reflectionShader =
		new	Shader("assets/shaders/reflection_refraction/reflection.vert",
			"assets/shaders/reflection_refraction/reflection.frag");
	blendingShader =
		new	Shader("assets/shaders/blend/blend.vert",
			"assets/shaders/blend/blend.frag");
	depthShader =
		new Shader("assets/shaders/shadowMap/depthShader.vert",
			"assets/shaders/shadowMap/depthShader.frag");
	depthQuadShader =
		new Shader("assets/shaders/shadowMap/depthQuadShader.vert",
			"assets/shaders/shadowMap/depthQuadShader.frag");

	// Init directional light
	Light::initDirectionalLight(&light);
	// Init spotlight:
	Light::initSpotLight(&light, camera);
	// Init point light's data:
	num_of_pointlights = 4;
	for (int i = 0; i < num_of_pointlights; i++) {
		Light::initPointLight(&light,i);
	}
	light[2]->position = glm::vec3(2.28f, 3.63f, 6.64f);
	light[3]->position = glm::vec3(0.62f, 3.63f, 6.64f);
	light[4]->position = glm::vec3(-5.88f, 5.64f, -14.91f);
	light[5]->position = glm::vec3(8.89f, 5.64f, -14.91f);
	
	// Cubemap and Skybox initialization
	std::vector<std::string> faces{
		"assets/textures/skybox/right.jpg",
		"assets/textures/skybox/left.jpg",
		"assets/textures/skybox/top.jpg",
		"assets/textures/skybox/bottom.jpg",
		"assets/textures/skybox/front.jpg",
		"assets/textures/skybox/back.jpg"
	};

	cubemapTexture = Model::loadCubemap(faces);

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	// skybox VAO
	unsigned int skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	// Loads all the geometry into the GPU
	buildGeometry();

	// Create shadow map frame buffer
	configureDepthMap();

	return true;
}

unsigned int loadTexture(const char *path) {
	unsigned int id;
	// Creates the texture on GPU
	glGenTextures(1, &id);
	// Loads the texture
	int textureWidth, textureHeight, numberOfChannels;
	// Flips the texture when loads it because in opengl the texture coordinates are flipped
	stbi_set_flip_vertically_on_load(true);
	// Loads the texture file data
	unsigned char *data = stbi_load(path, &textureWidth, &textureHeight, &numberOfChannels, 0);
	if (data) {
		// Gets the texture channel format
		GLenum format;
		switch (numberOfChannels) {
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		}
		// Binds the texture
		glBindTexture(GL_TEXTURE_2D, id);
		// Creates the texture
		glTexImage2D(GL_TEXTURE_2D, 0, format, textureWidth, textureHeight, 0, format, GL_UNSIGNED_BYTE, data);
		// Creates the texture mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set the filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "ERROR:: Unable to load texture " << path << std::endl;
		glDeleteTextures(1, &id);
	}
	// We dont need the data texture anymore because is loaded on the GPU
	stbi_image_free(data);

	return id;
}

// --------------------------------------------------------------------------------------

void initSceneModels() {
	// Load pointlights models. Indices 0 and 1 are for directional light and spotlight
	Model::loadModel("assets/models/scene/light1.obj", &model, pickColor, POINTLIGHT);
	model[0]->lightIndex = 2;
	Model::loadModel("assets/models/scene/light2.obj", &model, pickColor, POINTLIGHT);
	model[1]->lightIndex = 3;
	model[1]->transformations.translate = glm::vec3(2.28f, 3.3f, -3.78f);
	Model::loadModel("assets/models/scene/light3.obj", &model, pickColor, POINTLIGHT);
	model[2]->lightIndex = 4;
	Model::loadModel("assets/models/scene/light4.obj", &model, pickColor, POINTLIGHT);
	model[3]->lightIndex = 5;
	unsigned int texture = loadTexture("assets/textures/wall.jpg"), specMap;

	// Scenario
	Model::loadModel("assets/models/scene/ground.obj", &model, pickColor, MODEL);
	model[4]->albedo = loadTexture("assets/textures/grass.jpg");
	model[4]->material.material_type = BLINN_PHONG;
	Model::loadModel("assets/models/scene/backwall.obj", &model, pickColor, MODEL);
	Model::loadModel("assets/models/scene/sidewall1.obj", &model, pickColor, MODEL);
	Model::loadModel("assets/models/scene/sidewall2.obj", &model, pickColor, MODEL);
	Model::loadModel("assets/models/scene/frontwall.obj", &model, pickColor, MODEL);
	model[5]->albedo = model[6]->albedo = model[7]->albedo = model[8]->albedo = texture;
	model[5]->material.material_type = model[6]->material.material_type = model[7]->material.material_type = model[8]->material.material_type = OREN_NAYAR;
	Model::loadModel("assets/models/scene/chart.obj", &model, pickColor, MODEL);
	model[9]->albedo = loadTexture("assets/textures/chart.png");
	model[9]->material.material_type = OREN_NAYAR;
	model[4]->pickColor[0] = model[5]->pickColor[0] = model[6]->pickColor[0] = model[7]->pickColor[0] = model[8]->pickColor[0] = 0.0f;
	Model::loadModel("assets/models/scene/house.obj", &model, pickColor, MODEL);
	model[10]->albedo = loadTexture("assets/textures/house.jpeg");
	model[10]->material.ambient = glm::vec3(0.5, 0.5, 0.5);
	model[10]->material.diffuse = glm::vec3(0.0078, 0.22, 0.81);
	model[10]->material.specular = glm::vec3(0.49, 0.50, 0.69);
	model[10]->material.shininess = 2.5f;
	model[10]->material.material_type = COOK_TORRANCE;
	// Some models
	Model::loadModel("assets/models/scene/catright.obj", &model, pickColor, MODEL);
	model[11]->albedo = loadTexture("assets/textures/Cat_diffuse.jpg");
	Model::loadModel("assets/models/scene/catleft.obj", &model, pickColor, MODEL);
	model[12]->albedo = loadTexture("assets/textures/Cat_diffuse.jpg");
	//Cats wired settings
	model[11]->material.ambient = model[12]->material.ambient = glm::vec3(0.34f, 0.99f, 0.31f);
	model[11]->material.diffuse = model[12]->material.diffuse = glm::vec3(0.85f, 0.79f, 0.31f);
	model[11]->material.specular = model[12]->material.specular = glm::vec3(0.88f, 0.98, 0.0f);
	model[11]->material.roughness = 0.318f;
	model[12]->material.roughness = 0.193f;

	model[11]->material.material_type = model[12]->material.material_type = COOK_TORRANCE;

	texture = loadTexture("assets/textures/box_diff.png");
	specMap = loadTexture("assets/textures/box_spec.png");
	// Load boxes
	for (int i = 0; i < 6; i++) {
		Model::loadModel("assets/models/scene/box" + std::to_string(i + 1) + ".obj", &model, pickColor, MODEL);
		model[13 + i]->albedo = texture;
		model[13 + i]->specMap = specMap;
	}

	// Boxes wired settings
	model[13]->material.diffuse_type = model[14]->material.diffuse_type = model[15]->material.diffuse_type = COLOR;
	model[16]->material.spec_type = model[17]->material.spec_type = model[18]->material.spec_type = 1;
	model[14]->material.material_type = model[18]->material.material_type = OREN_NAYAR;
	model[15]->material.material_type = model[16]->material.material_type = COOK_TORRANCE;
	model[13]->material.ambient = model[14]->material.ambient = model[15]->material.ambient = glm::vec3(0.0, 0.45, 0.90);
	model[13]->material.diffuse = model[14]->material.diffuse = model[15]->material.diffuse = glm::vec3(0.93, 0.31, 0.82);
	model[13]->material.specular = model[14]->material.specular = model[15]->material.specular = glm::vec3(0.5, 1, 0.5);
	model[13]->material.shininess = 2.1f;
	model[15]->material.roughness = 0.288f;
	model[14]->material.roughness = 0.923f;
	model[16]->material.ambient = model[17]->material.ambient = model[18]->material.ambient = glm::vec3(1.0f);
	model[16]->material.diffuse = model[17]->material.diffuse = model[18]->material.diffuse = glm::vec3(1.0f);
	model[16]->material.specular = model[17]->material.specular = model[18]->material.specular = glm::vec3(1.0f, 1.0f, 0.0f);
	model[17]->material.shininess = 2.1f;
	model[16]->material.roughness = 0.288f;
	model[18]->material.roughness = 0.923f;

	// Lights wired settings

	light[0]->direction = glm::vec3(20.4f, 22.8f, -21.72f);
	light[0]->material.ambient = glm::vec3(0.3f);
	light[0]->material.diffuse = glm::vec3(0.39f);
	light[0]->material.specular = glm::vec3(0.58f);

	light[1]->material.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
	light[1]->material.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	light[1]->material.specular = glm::vec3(1.0f, 0.0f, 1.0f);
	light[1]->attenuation.constant = 0.956f;
	light[1]->attenuation.linear = 0.118f;
	light[1]->attenuation.quadratic = 0.0f;
	light[1]->cutOff = 0.9887f;
	light[1]->outerCutOff = 0.9620f;


	light[2]->material.ambient = light[3]->material.ambient = glm::vec3(0.89, 0.04, 1);
	light[2]->material.diffuse = light[3]->material.diffuse = glm::vec3(0.93, 1, 0.73);
	light[2]->material.specular = light[3]->material.specular = glm::vec3(0.0, 1, 0);

	light[2]->position = glm::vec3(6.72, 3.91, -6.94);
	light[3]->position = glm::vec3(6.72, 3.91, -3.84);

	light[2]->attenuation.constant = light[3]->attenuation.constant = 0.164f;
	light[2]->attenuation.linear = light[3]->attenuation.linear = 0.0f;
	light[2]->attenuation.quadratic = light[3]->attenuation.quadratic = 0.426f;

	light[4]->material.ambient = light[5]->material.ambient = glm::vec3(0.6, 0.77, 0.83);
	light[4]->material.diffuse = light[5]->material.diffuse = glm::vec3(0.88, 0.0, 1);
	light[4]->material.specular = light[5]->material.specular = glm::vec3(0.56, 0.39, 0.63);

	light[4]->attenuation.constant = light[5]->attenuation.constant = 1.154f;
	light[4]->attenuation.linear = light[5]->attenuation.linear = 0.0f;
	light[4]->attenuation.quadratic = light[5]->attenuation.quadratic = 0.233f;

	texture = loadTexture("assets/textures/grass.png");
	// Load herbs
	for (int i = 0; i < 24; i++) {
		Model::loadModel("assets/models/scene/herb" + std::to_string(i + 1) + ".obj", &model, pickColor, MODEL);
		model[19 + i]->albedo = texture;
		model[19 + i]->pickColor[0] = 0.0f;
	}
	
	// Model's set up for parallax and normal mapping

	Model::loadModel("assets/models/wall_new.obj", &model, pickColor, MODEL);
	model[43]->albedo = loadTexture("assets/textures/brickwall.jpg");
	model[43]->normalMap = loadTexture("assets/textures/brickwall_normal.jpg");
	model[43]->material.norm_type = 1;
	model[43]->transformations.quat = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	model[43]->transformations.translate = glm::vec3(7.25f, 1.13f, -8.43f);

	Model::loadModel("assets/models/wall_new2.obj", &model, pickColor, MODEL);
	model[44]->albedo = loadTexture("assets/textures/bricks2.jpg");
	model[44]->normalMap = loadTexture("assets/textures/bricks2_normal.jpg");
	model[44]->depthMap = loadTexture("assets/textures/bricks2_disp.jpg");
	model[44]->material.norm_type = 1;
	model[44]->material.depth_type = 1;
	model[44]->transformations.quat = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	model[44]->transformations.translate = glm::vec3(7.25f, 1.13f, -5.32f);

	Model::loadModel("assets/models/wall_new3.obj", &model, pickColor, MODEL);
	model[45]->albedo = loadTexture("assets/textures/toy_box_diffuse.png");
	model[45]->normalMap = loadTexture("assets/textures/toy_box_normal.png");
	model[45]->depthMap = loadTexture("assets/textures/toy_box_disp.png");
	model[45]->material.norm_type = 1;
	model[45]->material.depth_type = 1;
	model[45]->transformations.quat = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	model[45]->transformations.translate = glm::vec3(7.25f, 1.13f, -2.32f);

	model[45]->material.material_type = model[44]->material.material_type =
		model[43]->material.material_type = BLINN_PHONG;

	// Model's set up for reflaction and refraction

	Model::loadModel("assets/models/icoSphere.obj", &model, pickColor, MODEL);
	model[46]->material.material_type = REFLECTION;
	model[46]->albedo = loadTexture("assets/textures/bricks2.jpg");
	model[46]->normalMap = loadTexture("assets/textures/bricks2_normal.jpg");
	model[46]->depthMap = loadTexture("assets/textures/bricks2_disp.jpg");
	model[46]->material.norm_type = 0;
	model[46]->material.depth_type = 0;
	model[46]->material.diffuse_type = COLOR;
	model[46]->transformations.scale = 2.886f;
	model[46]->transformations.translate = glm::vec3(6.35f, 19.10f, 0.0f);
	model[46]->material.reflection = 1.0f;

	Model::loadModel("assets/models/uvSphere.obj", &model, pickColor, MODEL);
	model[47]->material.material_type = REFLECTION;
	model[47]->albedo = loadTexture("assets/textures/bricks2.jpg");
	model[47]->normalMap = loadTexture("assets/textures/bricks2_normal.jpg");
	model[47]->depthMap = loadTexture("assets/textures/bricks2_disp.jpg");
	model[47]->material.norm_type = 0;
	model[47]->material.depth_type = 0;
	model[47]->material.diffuse_type = COLOR;
	model[47]->transformations.scale = 2.886f;
	model[47]->transformations.translate = glm::vec3(-6.35f, 17.34f, 0.0f);
	model[47]->material.reflection = 0.5f;

	Model::loadModel("assets/models/cube_blend.obj", &model, pickColor, MODEL);
	model[48]->material.material_type = BLINN_PHONG;
	model[48]->albedo = loadTexture("assets/textures/marble.jpg");
	model[48]->transformations.translate = glm::vec3(-9.89f, 2.01f, -1.54f);

	Model::loadModel("assets/models/cube_blend.obj", &model, pickColor, MODEL);
	model[49]->material.material_type = BLINN_PHONG;
	model[49]->albedo = loadTexture("assets/textures/marble.jpg");
	model[49]->transformations.translate = glm::vec3(-6.75f, 2.01f, -6.90f);

	// Model's set up for semi-transparent objects
	Model::loadModel("assets/models/window.obj", &model, pickColor, MODEL);
	model[50]->material.material_type = BLEND;
	model[50]->albedo = loadTexture("assets/textures/blending_transparent_window.png");
	model[50]->transformations.translate = glm::vec3(-11.87f, 1.66f, -2.92f);

	Model::loadModel("assets/models/window.obj", &model, pickColor, MODEL);
	model[51]->material.material_type = BLEND;
	model[51]->albedo = loadTexture("assets/textures/blending_transparent_window.png");
	model[51]->transformations.translate = glm::vec3(-8.91f, 1.67f, -0.53f);

	Model::loadModel("assets/models/window.obj", &model, pickColor, MODEL);
	model[52]->material.material_type = BLEND;
	model[52]->albedo = loadTexture("assets/textures/blending_transparent_window.png");
	model[52]->transformations.translate = glm::vec3(-8.86f, 1.66f, -3.97f);

	Model::loadModel("assets/models/window.obj", &model, pickColor, MODEL);
	model[53]->material.material_type = BLEND;
	model[53]->albedo = loadTexture("assets/textures/blending_transparent_window.png");
	model[53]->transformations.translate = glm::vec3(-6.28f, 1.66f, -2.92f);

	Model::loadModel("assets/models/window.obj", &model, pickColor, MODEL);
	model[54]->material.material_type = BLEND;
	model[54]->albedo = loadTexture("assets/textures/blending_transparent_window.png");
	model[54]->transformations.translate = glm::vec3(-5.80f, 1.66f, -5.90f);	
}

void buildGeometry(){

	// Initialize Scene
	initSceneModels();
	
	std::vector<Model*>::iterator it = model.begin();
	for (it; it != model.end();it++){
		// Creates on GPU the vertex array
		glGenVertexArrays(1, &(*it)->VAO);
		// Creates on GPU the vertex buffer object
		glGenBuffers(1, &(*it)->VBO);
		// Binds the vertex array to set all the its properties
		glBindVertexArray((*it)->VAO);
		// Binds the buffer object to current VAO 
		glBindBuffer(GL_ARRAY_BUFFER, (*it)->VBO);
		// Sets the buffer geometry data	
		glBufferData(GL_ARRAY_BUFFER, (*it)->verticesData.size()*sizeof(Data), &(*it)->verticesData[0], GL_STATIC_DRAW);
		// Sets the vertex attributes
		// Position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)0);
		// Texture
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6* sizeof(float)));
		// Normal
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
		// Tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
		// Bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
		//Finish binding attributes to VAO
		glBindVertexArray(0);
	}
}

void pick() {
		double cursorX, cursorY;

		// Clear the screen in white
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwGetCursorPos(window, &cursorX, &cursorY);
		cursorX = float(cursorX);
		cursorY = windowHeight - float(cursorY);

		pickingShader->use();
		pickingShader->setInt("myTexture", 0);
		pickingShader->setMat4("projection", Projection);
		pickingShader->setMat4("view", View);
		
		// Projection matrix: 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
		// View Matrix
		View = camera.getWorldToViewMatrix();
		// Model Matrix
		ModelMatrix = glm::mat4(1.0f);
		
		std::vector<Model*>::iterator it = model.begin();
		for (it; it != model.end();it++){
			if ((*it)->type == POINTLIGHT){
				ModelMatrix = glm::translate(ModelMatrix, light[(*it)->lightIndex]->position);
				pickingShader->setMat4("model", ModelMatrix);
			}
			else{
				ModelMatrix = glm::translate(ModelMatrix, (*it)->transformations.translate);
				ModelMatrix *= Model::toMat4((*it)->transformations.quat);
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3((*it)->transformations.scale, (*it)->transformations.scale, (*it)->transformations.scale));
				pickingShader->setMat4("model", ModelMatrix);

			}
			// Set pick color for models
			pickingShader->setVec4("pickingColor", (*it)->pickColor[0], (*it)->pickColor[1], (*it)->pickColor[2], 1.0f);
			// Binds the vertex array to be drawn
			glBindVertexArray((*it)->VAO);
			// Draw the triangles 
			glDrawArrays(GL_TRIANGLES, 0, (*it)->verticesData.size());
			glBindVertexArray(0);
			ModelMatrix = glm::mat4(1.0f);
		}

		// Wait until all the pending drawing commands are really done. SLOW
		glFlush();
		glFinish();

		// Read the pixel where click happened. SLOW 
		float readPixel[4];
		glReadPixels((int)cursorX, (int)cursorY, 1, 1, GL_RGBA, GL_FLOAT, &readPixel);
		// Round color floats to be able to compare
		readPixel[0] = roundf(readPixel[0] * 100) / 100;
		it = model.begin();
		int i = 0;
		for (it; it != model.end(); it++){
			if (abs(readPixel[0] - (*it)->pickColor[0]) < 0.001 && readPixel[0] != 0.0f){
				pickedIndex = i;
			}
			i++;
		}
		// Uncomment these lines to see picking shader in effect
		//glfwSwapBuffers(window);
}

void render(){
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    // Clears the color and depth buffers from the frame buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// View matrix
	View = camera.getWorldToViewMatrix();
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// World transformation
	ModelMatrix = glm::mat4(1.0f);
	// Depth sort for proper blending
	Model::sortWindows(&model, camera.position);

	// Draw models and lamps
	for (auto it = model.begin(); it != model.end(); it++){
		switch ((*it)->type) {
			case MODEL: {
				// Model transformations
				ModelMatrix = glm::translate(ModelMatrix, (*it)->transformations.translate);
				ModelMatrix *= Model::toMat4((*it)->transformations.quat);
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3((*it)->transformations.scale, (*it)->transformations.scale, (*it)->transformations.scale));
				// Select material illumination technique shader
				switch ((*it)->material.material_type) {
				case BLINN_PHONG: {
					Shader::
						configModelShader(blinnPhongShader, (*it), light, ModelMatrix,
							View, Projection, LightProjectionMatrix, camera, num_of_pointlights);
					break;
				}
				case OREN_NAYAR: {
					Shader::
						configModelShader(orenNayarShader, (*it), light, ModelMatrix,
							View, Projection, LightProjectionMatrix, camera, num_of_pointlights);
					break;
				}
				case COOK_TORRANCE: {
					Shader::
						configModelShader(cookTorranceShader, (*it), light, ModelMatrix,
							View, Projection, LightProjectionMatrix, camera, num_of_pointlights);
					break;
				}
				case REFLECTION: {
					glDisable(GL_BLEND);
					Shader::configModelShader(reflectionShader, (*it), light, ModelMatrix,
						View, Projection, LightProjectionMatrix, camera, num_of_pointlights);
					reflectionShader->setInt("skybox", 0);
					reflectionShader->setFloat("reflection", (*it)->material.reflection);
					reflectionShader->setFloat("material.material_reflective_index", (*it)->material.material_reflective_index);
					reflectionShader->setFloat("material.ambient_reflective_index", (*it)->material.ambient_reflective_index);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
					break;
				}
				case BLEND: {
					Shader::
						configModelShader(blendingShader, (*it), light, ModelMatrix,
							View, Projection, LightProjectionMatrix, camera, num_of_pointlights);
					break;
				}
				}

				// Diffuse Map -> 0: diff map 1: color
				glActiveTexture(GL_TEXTURE0);
				if ((*it)->material.diffuse_type == 0) {

					glBindTexture(GL_TEXTURE_2D, (*it)->albedo);
				}
				// Naive texture
				else {
					glBindTexture(GL_TEXTURE_2D, texture_white);
				}

				// Specular Map -> 0: color 1: spec map
				glActiveTexture(GL_TEXTURE1);
				if ((*it)->material.spec_type == 1) {
					glBindTexture(GL_TEXTURE_2D, (*it)->specMap);
				}
				// Naive texture
				else {
					glBindTexture(GL_TEXTURE_2D, texture_white);
				}

				// Normal Map -> 0: regular 1: normal map
				glActiveTexture(GL_TEXTURE2);
				if ((*it)->material.norm_type == 1) {
					glBindTexture(GL_TEXTURE_2D, (*it)->normalMap);
				}
				// Naive texture
				else {
					glBindTexture(GL_TEXTURE_2D, texture_blue);
				}
				// Depth Map -> 0: regular 1: Depth map
				glActiveTexture(GL_TEXTURE3);
				if ((*it)->material.depth_type == 1) {
					glBindTexture(GL_TEXTURE_2D, (*it)->depthMap);
				}
				// Naive texture
				else {
					glBindTexture(GL_TEXTURE_2D, texture_black);
				}
				// Shadow Map
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, depthMapTexture);


				break;
			}
			case POINTLIGHT: {
				// Translate using light's position linked to this model
				ModelMatrix = glm::translate(ModelMatrix, light[(*it)->lightIndex]->position);
				lampShader->use();
				lampShader->setInt("myTexture", 0);
				lampShader->setMat4("projection", Projection);
				lampShader->setMat4("view", View);
				lampShader->setMat4("model", ModelMatrix);
				lampShader->setVec3("material.ambient", light[(*it)->lightIndex]->material.ambient);
				lampShader->setVec3("material.diffuse", light[(*it)->lightIndex]->material.diffuse);
				lampShader->setVec3("material.specular", light[(*it)->lightIndex]->material.specular);
			}
		}
		// Binds the vertex array to be drawn
		glBindVertexArray((*it)->VAO);
		// Renders the geometry
		glDrawArrays(GL_TRIANGLES,0, (*it)->verticesData.size());
		glBindVertexArray(0);
		glEnable(GL_BLEND);
		ModelMatrix = glm::mat4(1.0f);
	}

	// Draw skybox as last
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	skyboxShader->use();
	skyboxShader->setInt("skybox", 0);
	skyboxShader->setMat4("view", glm::mat4(glm::mat3(camera.getWorldToViewMatrix())));
	skyboxShader->setMat4("projection", Projection);

	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
}

void renderQuad(){
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void renderToDepthMap() {
	glCullFace(GL_FRONT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 1. render depth of scene to texture (from light's perspective)
	// --------------------------------------------------------------

	
	float near_plane = 1.0f, far_plane = 60.5f;
	glm::mat4 lightProjection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(light[0]->direction, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	LightProjectionMatrix = lightProjection * lightView;
	depthShader->use();
	depthShader->setMat4("LightProjectionMatrix", LightProjectionMatrix);
	// Sets the viewport to fit the texture size
	glViewport(0, 0, DEPTH_BUFFER_SIZE, DEPTH_BUFFER_SIZE);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);


	// Render depth of scene to texture (from light's perspective)
		// Draw models and lamps
		for (auto it = model.begin(); it != model.end(); it++) {
			ModelMatrix = glm::mat4(1.0f);
			// Model transformations
			ModelMatrix = glm::translate(ModelMatrix, (*it)->transformations.translate);
			ModelMatrix *= Model::toMat4((*it)->transformations.quat);
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3((*it)->transformations.scale, (*it)->transformations.scale, (*it)->transformations.scale));
			depthShader->setMat4("model", ModelMatrix);
			// Send diffuse map to discard fragments
			depthShader->setInt("diffMap", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, (*it)->albedo);
			// Binds the vertex array to be drawn
			glBindVertexArray((*it)->VAO);
			// Renders the geometry
			glDrawArrays(GL_TRIANGLES, 0, (*it)->verticesData.size());
			glBindVertexArray(0);
		}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth, windowHeight);

	glCullFace(GL_BACK);

	// Render depth map quad for visual debugging
	if (cameraView) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Render depth map
		depthQuadShader->use();
		depthQuadShader->setInt("depthMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		renderQuad();
	}
}

void update(){
    // Loop until something tells the window that it has to be closed
    while (!glfwWindowShouldClose(window)){
		// Checks for keyboard inputs
		processKeyboardInput(window);
		// Render from camera perspective
		renderToDepthMap();
		// Render everything
		if (!cameraView)
			render();
		//Draw AntTweakBar:
		TwDraw();
		// Update UI
		ui->updateFromUI(pickedIndex, &model, &light, &cameraView);
		// Swap the buffer
		glfwSwapBuffers(window);
		// Check and call events
		glfwPollEvents();
    }
}

int main(int argc, char const *argv[]){
    // Initialize all the app components
   if (!init()){
        // Something went wrong
        std::cin.ignore();
        return -1;
    }
   
    std::cout << "=====================================================" << std::endl
              << "        Press Escape to close the program            " << std::endl
              << "=====================================================" << std::endl;

    // Starts the app main loop
    update();
    
	std::vector<Model*>::iterator it = model.begin();
	while (it != model.end()) {
		// Deletes all the vertex arrays from GPU
		glDeleteVertexArrays(1, &(*it)->VAO);
		// Deletes all the vertex objects from GPU
		glDeleteBuffers(1, &(*it)->VBO);
		it++;
	}

    // Destroy the shader
    delete lampShader;
	delete pickingShader;
	delete lightShader;
	//Destroy tweakbar:
	delete ui;
	//Stops Tweakbar:
	TwTerminate();
    // Stops the glfw program
    glfwTerminate();
	
    return 0;
}

