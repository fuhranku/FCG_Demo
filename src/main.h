
#include <glad/glad.h> // Glad has to be included before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <stb_image.h>
#include "Shader.h"
#include "UserInterface.h"
#include "Model.h"
#include "Camera.h"
#include "Light.h"

#define px(x) x * ( windowWidth / windowHeight )
#define DEPTH_BUFFER_SIZE 1024

/*!
 * Handles the window resize
 * @param{GLFWwindow} window pointer
 * @param{int} new width of the window
 * @param{int} new height of the window
 * */
void resize(GLFWwindow *window, int width, int height);

/*!
 * This function initializes the AntTweakBar
 * interface inside the window
 * created by OpenGL.
 * @returns{bool} true if it was successfully initialized.
 **/
bool initATB();

/*!
 * Initialize the glfw library
 * @returns{bool} true if everything goes ok
 * */
bool initWindow();

/*!
 * Remember that Glad it's an abstraction layer to avoid
 * changing instructions if OpenGL version changes
 * from one project to another
 * Initialize the glad library
 * @returns{bool} true if everything goes ok
 * */
bool initGlad();

/*!
 * Initialize the opengl context
 * */
void initGL();

/*!
 * Builds all the geometry buffers and
 * loads them up into the GPU
 * (Builds a simple triangle)
 * */
void buildGeometry();

/*!
 * Loads a texture into the GPU
 * @param{const char} path of the texture file
 * @returns{unsigned int} GPU texture index
 * */
unsigned int loadTexture(const char *path);

/*!
 * Initialize everything
 * @returns{bool} true if everything goes ok
 * */
bool init();

/*!
 * Render Function
 * */
void render();

/*!
 * App's main loop
 * */
void update();

/*!
 * Function to pick models on the screen using
 * color picking technique
 */
void pick();

/*! Initializes Frame buffer Object
 * to implement shadow mapping
*/
void configureDepthMap();

/*! 
 * Render scene from camera perspective to a depth map
*/
void renderToDepthMap();

/*!
 * App starting point
 * @param{int} number of arguments
 * @param{char const *[]} running arguments
 * @returns{int} app exit code
 * */
int main(int argc, char const *argv[]);