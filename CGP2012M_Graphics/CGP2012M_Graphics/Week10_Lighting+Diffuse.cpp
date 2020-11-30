// standard library header files
#include <iostream>
#include <vector>

// GL debugging header.
#include "GLerror.h"
// SDL functionality.
#include "SDL_Start.h"
// Almost everything is derrived from this class.
#include "Triangle_T.h"
// Background rendering.
#include "Square.h"
// Camera controller.
#include "Camera.h"
// Models are derrived from this class.
#include "Model.h"
// instead of relying on raw vertices, this class parses data from specified .obj files.
#include "ModelLoaderClass.h"
#ifndef TEXTURECLASS_H
#define TEXTURECLASS_H
#ifndef SHADERCLASS_H
#define SHADERCLASS_H

// include glew before sdl.
#ifndef GLEW_H
#define GLEW_H
#include <GL/glew.h>
#include "windows.h"

// sdl.
#ifndef SDL_H
#define SDL_H
#include "SDL.h"
#include "SDL_image.h"

// basic variables.
SDL_Event event;
SDL_Window *win;
bool windowOpen = true;
bool isFullScreen = false;
float angle = 0;

// window aspect, height and width.
int w;
int h;
float aspect;
int left;
int newwidth;

// transform matrices for first model.
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;
glm::mat4 normalMatrix;

glm::mat4 rotate;
glm::mat4 scale;
glm::mat4 backgroundTranslate;
glm::mat4 backgroundScale;
glm::vec3 b_scaleFactor;
glm::mat4 modelRotate;
glm::mat4 modelScale;
glm::mat4 modelTranslate;

// transform matrices for second model.
glm::mat4 normalMatrix2;
glm::mat4 modelRotate2;
glm::mat4 modelScale2;
glm::mat4 modelTranslate2;

// create camera object.
Camera cam;

// lighting variables.
glm::vec3 lightCol;
glm::vec3 lightPosition;

// can't change this.
bool flag = true;

//==========================================================================================================================================
// Handle User Input
//==========================================================================================================================================
void handleInput()
{
	// poll for SDL events.
	if (SDL_PollEvent(&event))
	{
		// when SDL is shutdown, quit application
		if (event.type == SDL_QUIT)
		{
			windowOpen = false;
		}
		// handle when window is resized.
		if (event.type == SDL_WINDOWEVENT)
		{
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				std::cout << "Window resized w:" << w << " h:" << h << std::endl;
				SDL_GetWindowSize(win, &w, &h);
				newwidth = h * aspect;
				left = (w - newwidth) / 2;
				glViewport(left, 0, newwidth, h);
				break;
			default:
				break;
			}
		}

		// handle all key events.
		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
				// model movement.
			case SDLK_UP:
				// translate cube up when up key pressed.
				modelTranslate2 = glm::translate(modelTranslate2, glm::vec3((float)cos(angle) * 0.02f, (float)sin(angle) * 0.02f, 0.0f));
				break;
			case SDLK_LEFT:
				// change the angle (45 degrees) clockwise at which to translate the model by.
				angle += glm::radians(45.0f);
				rotate = glm::rotate(rotate, glm::radians(10.0f), glm::vec3(0, 0, 1));
				break;
			case SDLK_RIGHT:
				// change the angle (45 degrees) anti-clockwise at which to translate the model by.
				angle -= glm::radians(45.0f);
				rotate = glm::rotate(rotate, glm::radians(-10.0f), glm::vec3(0, 0, 1));
				break;

				// camera movement.
			case SDLK_q:
				// move camera 'forward' in the -ve z direction
				cam.camZPos -= cam.camSpeed;
				break;
			case SDLK_e:
				// move camera 'backwards' in +ve z direction
				cam.camZPos += cam.camSpeed;
				break;
			case SDLK_a:
				// move camera left
				// move camera target with position
				cam.camXPos -= cam.camSpeed;
				cam.camXTarget -= cam.camSpeed;
				break;
			case SDLK_d:
				// move camera right
				// move camera target with position
				cam.camXPos += cam.camSpeed;
				cam.camXTarget += cam.camSpeed;
				break;
			case SDLK_w:
				// move camera up
				cam.camYPos += cam.camSpeed;
				cam.camYTarget += cam.camSpeed;
				break;
			case SDLK_s:
				// move camera down
				cam.camYPos -= cam.camSpeed;
				cam.camYTarget -= cam.camSpeed;
				break;
			
				// lighting.
			case SDLK_f:
				// move light left.
				lightPosition.x -= 0.1f;
				break;
			case SDLK_h:
				// move light right.
				lightPosition.x += 0.1f;
				break;
			case SDLK_g:
				// move light up.
				// setting this to one to seem to fix the problem of the light source not moving.
				lightPosition.y += 1.1f;
				// move light down.
			case SDLK_b:
				lightPosition.y -= 0.1f;
				break;
			default:
				break;
			}
		}
	}
}

//==========================================================================================================================================
int main(int argc, char *argv[]) {
//==========================================================================================================================================
	
	// initialise SDL.
	SDL_Start sdl;
	// create window context.
	SDL_GLContext context = sdl.Init();
	win = sdl.win;

	// get window size.
	SDL_GetWindowSize(win, &w, &h);
	// declare viewport.
	glViewport(0, 0, w, h);
	// delcare aspect ratio.
	aspect = (float)w / (float)h;

	// initialise error object.
	GLerror glerr;
	int errorLabel;

	// initialise glew.
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	//register debug callback (returns specific error label to debug where the code is failing).
	if (glDebugMessageCallback)
	{
		std::cout << "Registering OpenGL Debug callback function" << std::endl;
		glDebugMessageCallback(glerr.openglCallbackFunction, &errorLabel);
		glDebugMessageControl(GL_DONT_CARE,
			GL_DONT_CARE,
			GL_DONT_CARE,
			0,
			NULL,
			true);
	}

	// openGL specific data.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// initialise background object.
	Square background;

	// initialise first and second model.
	Model model;
	Model model2;

	// initialise model loader and model loader 2.
	ModelImport modelLoader; 
	ModelImport modelLoader2;

	// store model in model loader variable. (load the sphere).
	modelLoader.LoadOBJ2("..//..//Assets//Models//blenderSphere.obj", model.vertices, model.texCoords, model.normals, model.indices);

	// store model in model loader variable. (load the cube). 
	modelLoader2.LoadOBJ2("..//..//Assets//Models//blenderCube.obj", model2.vertices, model2.texCoords, model2.normals, model2.indices);

	errorLabel = 0;

	// initialise array to store textures.
	Texture texArray[4] = { 0, 0, 0, 0 };

	// load background texture and set buffers.
	texArray[0].load("..//..//Assets//Textures//space.png");
	texArray[0].setBuffers();

	// load perlin noise texture and set buffers.
	texArray[1].load("..//..//Assets//Textures//Perlin_Noise.jpg");
	texArray[1].setBuffers();

	// load lava texture and set buffers.
	texArray[2].load("..//..//Assets//Textures//lava.png");
	texArray[2].setBuffers();

	// load ice texture and set buffers.
	texArray[3].load("..//..//Assets//Textures//ice.jpg");
	texArray[3].setBuffers();

	errorLabel = 2;

	// openGL set buffers for background and both models.
	background.setBuffers();
	model.setBuffers();
	model2.setBuffers();

	errorLabel = 3;

	// set uniform variables for model.
	int modelLocation;
	int viewLocation;
	int projectionLocation;
	int importModelLocation;
	int importViewLocation;
	int importProjectionLocation;
	int backgroundColourLocation;
	int ambientIntensityLocation;
	int modelAmbientLocation;
	int lightColLocation;
	int normalMatrixLocation;
	int lightPositionLocation;

	int firstTextureLocation;
	int secondTextureLocation;
	int thirdTextureLocation;

	int backgroundTextureLocation;

	float timeLocation;

	// GL ints for tracking time.
	GLuint currentTime = 0;
	GLuint lastTime = 0;
	GLuint elapsedTime = 0;

	// lighting postion.
	lightPosition = glm::vec3(1.0f, 0.0f, 0.5f);

	// light colour setting. changing values simulates different light sources.
	glm::vec3 lightColour = glm::vec3(1.0f, 1.0f, 0.98f);

	// light for the background
	float ambientIntensity = 1.0f;
	// light distance setting
	lightCol = glm::vec3(1.0f, 1.0f, 0.98f);

	// 3D perspective projection.
	projectionMatrix = glm::perspective(glm::radians(50.0f), (float)w / (float)h, 0.1f, 100.0f);

	// basic declaration of a 4x4 view matrix (1.0f value down the middle of matrix).
	viewMatrix = glm::mat4(1.0f);

	// basic declaration of a 4x4 object transformation matrix for the background.
	backgroundScale = glm::mat4(1.0f);
	backgroundTranslate = glm::mat4(1.0f);

	// basic declaration of 3 4x4 object transormation matrices for sphere model (scale, rotation, position).
	modelScale = glm::mat4(1.0f);
	modelRotate = glm::mat4(1.0f);
	modelTranslate = glm::mat4(1.0f);

	// basic declaration of 3 4x4 object transormation matrices for cube model (scale, rotation, position).
	modelScale2 = glm::mat4(1.0f);
	modelRotate2 = glm::mat4(1.0f);
	modelTranslate2 = glm::mat4(1.0f);

	// scale background and translate background.
	b_scaleFactor = { 60.0f, 50.0f, 1.0f };
	backgroundScale = glm::scale(backgroundScale, glm::vec3(b_scaleFactor));
	backgroundTranslate = glm::translate(backgroundTranslate, glm::vec3(0.0f, 0.0f, -2.0f));

	// initially scale and place sphere model in the scene.
	modelScale = glm::scale(modelScale, glm::vec3(0.7f, 0.7f, 0.7f));
	modelTranslate = glm::translate(modelTranslate, glm::vec3(0.0f, 0.0f, 0.0f));

	// initially scale and place cube model in the scene.
	modelScale2 = glm::scale(modelScale2, glm::vec3(0.7f, 0.7f, 0.7f));
	modelTranslate2 = glm::translate(modelTranslate2, glm::vec3(0.0f, 0.0f, -1.0f));

	errorLabel = 4;

	// game loop.
	while (windowOpen)
	{
		// OpenGL, specifify clear values for colour buffers.
		glClearColor(1.0f, 1.0f, 1.0f, 1);
		// clear buffers to preset values.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// process user input.
		handleInput();
		// update camera position.
		cam.updateCamera();

		// calculate currently elapsed time.
		currentTime = SDL_GetTicks();
		elapsedTime = currentTime - lastTime;
		lastTime = currentTime;

		// update camera view matrix. Set position, target, up direction (would be nice to work out rotation).
		viewMatrix = glm::lookAt(glm::vec3(cam.camXPos, cam.camYPos, cam.camZPos), cam.cameraTarget, cam.cameraUp);

		errorLabel = 5;

		//==================================================================================================================================
		// Background rendering
		//==================================================================================================================================

		// specify shader program to use for background.
		glUseProgram(background.shaderProgram);

		backgroundTextureLocation = glGetUniformLocation(model.shaderProgram, "backgroundTex");

		// specify lighting for background.
		backgroundColourLocation = glGetUniformLocation(background.shaderProgram, "uLightColour");
		glProgramUniform3fv(background.shaderProgram, backgroundColourLocation, 1, glm::value_ptr(lightCol));

		// specify light intensity for background.
		ambientIntensityLocation = glGetUniformLocation(background.shaderProgram, "uAmbientIntensity");
		glProgramUniform1f(background.shaderProgram, ambientIntensityLocation, ambientIntensity);

		// tells program location of uniforms variables for model, view and projection
		modelLocation = glGetUniformLocation(background.shaderProgram, "uModel");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(backgroundTranslate*backgroundScale));
		viewLocation = glGetUniformLocation(background.shaderProgram, "uView");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		projectionLocation = glGetUniformLocation(background.shaderProgram, "uProjection");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		
		glUniform1i(backgroundTextureLocation, 0);
		// tells program to bind shader.
		glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
		glBindTexture(GL_TEXTURE_2D, texArray[0].texture);
		// tells program to render to shader.
		background.render();
		//==================================================================================================================================

		//==================================================================================================================================
		// sphere model rendering
		//==================================================================================================================================

		glUseProgram(model.shaderProgram);

		firstTextureLocation = glGetUniformLocation(model.shaderProgram, "uPerlinTex");
		secondTextureLocation = glGetUniformLocation(model.shaderProgram, "uLavaTex");
		thirdTextureLocation = glGetUniformLocation(model.shaderProgram, "uIceTex");

		// time
		timeLocation = glGetUniformLocation(model.shaderProgram, "uTime");
		glProgramUniform1f(model.shaderProgram, timeLocation, currentTime);
		
		// lighting uniforms
		// get and set light colour and position uniform
		lightColLocation = glGetUniformLocation(model.shaderProgram, "lightCol");
		glUniform3fv(lightColLocation, 1, glm::value_ptr(lightColour));

		lightPositionLocation = glGetUniformLocation(model.shaderProgram, "lightPos");
		glUniform3fv(lightPositionLocation, 1, glm::value_ptr(lightPosition));

		// rotate models.
		modelRotate = glm::rotate(modelRotate, (float)elapsedTime / 2000, glm::vec3(0.0f, 1.0f, 0.0f));
		importModelLocation = glGetUniformLocation(model.shaderProgram, "uModel");
		glUniformMatrix4fv(importModelLocation, 1, GL_FALSE, glm::value_ptr(modelTranslate*modelRotate*modelScale));
		importViewLocation = glGetUniformLocation(model.shaderProgram, "uView");
		glUniformMatrix4fv(importViewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		importProjectionLocation = glGetUniformLocation(model.shaderProgram, "uProjection");
		glUniformMatrix4fv(importProjectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		// set normal matrix.
		normalMatrix = glm::transpose(glm::inverse(modelTranslate*modelRotate*modelScale * viewMatrix));

		// set the normalMatrix in the shaders
		glUniform1i(firstTextureLocation, 0);
		glUniform1i(secondTextureLocation, 1);
		glUniform1i(thirdTextureLocation, 2);
		normalMatrixLocation = glGetUniformLocation(model.shaderProgram, "uNormalMatrix");
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
		glBindTexture(GL_TEXTURE_2D, texArray[1].texture);
		glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
		glBindTexture(GL_TEXTURE_2D, texArray[2].texture);
		glActiveTexture(GL_TEXTURE0 + 2); // Texture unit 1
		glBindTexture(GL_TEXTURE_2D, texArray[3].texture);

		model.render();
		//==================================================================================================================================

		// update the window after everything has been rendered in the loop.
		SDL_GL_SwapWindow(sdl.win);

	}// end of game loop. 

	// delete context from memory. (cleanup).
	SDL_GL_DeleteContext(context);

	// quit SDL.
	SDL_Quit();
	return 0;
}
//==========================================================================================================================================


//==========================================================================================================================================
#endif
#endif
#endif
#endif
