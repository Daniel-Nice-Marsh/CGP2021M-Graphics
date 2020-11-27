// standard library header files
#include <iostream>
#include <vector>

// include shape and shader header files.
#include "GLerror.h"
#include "SDL_Start.h"
#include "Triangle_T.h"
#include "Circle.h"
#include "CircleTexture.h"
#include "Square.h"
#include "Camera.h"
#include "Cube.h"
#include "Model.h"
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

// include openGL.
#define GLM_FORCE_RADIANS // force glm to use radians.
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// basic variables.
SDL_Event event;
SDL_Window *win;
bool windowOpen = true;
bool isFullScreen = false;
float bubbleSpeed = -0.001f;
float radius;
float angle = 0;
// screen boundaries for collision tests.
float bX_r = 2.0f;
float bX_l = -2.0f;
float bY_t = 1.0f;
float bY_b = -1.0f;
// screen centre variables.
float centreX = 0.0f;
float centreY = 0.0f;
// window aspect, height and width.
int w;
int h;
float aspect;
int left;
int newwidth;
int newheight;

// transform matrices. (not sure why these are all intitialised globally.
glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;
glm::mat4 normalMatrix;

glm::mat4 translate;
glm::mat4 rotate;
glm::mat4 scale;
glm::mat4 backgroundTranslate;
glm::mat4 backgroundScale;
glm::vec3 b_scaleFactor;
glm::mat4 modelRotate;
glm::mat4 modelScale;
glm::mat4 modelTranslate;

glm::mat4 modelMatrix2;
glm::mat4 viewMatrix2;
glm::mat4 projectionMatrix2;
glm::mat4 normalMatrix2;

glm::mat4 translate2;
glm::mat4 rotate2;
glm::mat4 scale2;
glm::mat4 backgroundTranslate2;
glm::mat4 backgroundScale2;
glm::vec3 b_scaleFactor2;
glm::mat4 modelRotate2;
glm::mat4 modelScale2;
glm::mat4 modelTranslate2;

// create camera object.
Camera cam;

// camera object variables.
glm::vec3 camPos;
glm::vec3 camTarget;

// lighting variables.
glm::vec3 lightCol;
glm::vec3 lightPosition;
glm::vec3 viewPosition;
float ambientIntensity;

// what is this?
bool flag = true;

// function prototypes. (look these up).
CircleTexture updatePositions(CircleTexture c);
void handleInput();

//============================================================================================
int main(int argc, char *argv[]) {
//============================================================================================
	
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

	// load sphere texture and set buffers.
	texArray[1].load("..//..//Assets//Textures//bubble.png");
	texArray[1].setBuffers();

	// load cube texture and set buffers.
	texArray[2].load("..//..//Assets//Textures//lava.png");
	texArray[2].setBuffers();

	errorLabel = 2;

	// openGL set buffers for background and both models.
	background.setBuffers();
	model.setBuffers();
	model2.setBuffers();

	errorLabel = 3;

	// set uniform variables for cube model.
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

	/*
	set uniform variables for second sphere model.
	*/

	// GL int for tracking time?
	GLuint currentTime = 0;
	GLuint lastTime = 0;
	GLuint elapsedTime = 0;

	// lighting postion.
	lightPosition = glm::vec3(1.0f, 0.0f, 0.5f);

	// light colour setting (might get rid of this). changing values simulates different light sources.
	glm::vec3 lightColour = glm::vec3(1.0f, 1.0f, 0.98f);

	// light for the background
	ambientIntensity = 1.0f;
	// light distance setting
	lightCol = glm::vec3(1.0f, 1.0f, 0.98f);

	// 3D perspective projection.
	projectionMatrix = glm::perspective(glm::radians(50.0f), (float)w / (float)h, 0.1f, 100.0f);

	// basic declaration of a 4x4 view matrix (1.0f value down the middle of matrix).
	viewMatrix = glm::mat4(1.0f);

	// basic declaration of a 4x4 object transformation matrix for the background.
	backgroundScale = glm::mat4(1.0f);
	backgroundTranslate = glm::mat4(1.0f);

	// basic declaration of 3 4x4 object transormation matrices for cube model (scale, rotation, position).
	modelScale = glm::mat4(1.0f);
	modelRotate = glm::mat4(1.0f);
	modelTranslate = glm::mat4(1.0f);

	// basic declaration of 3 4x4 object transormation matrices for sphere model (scale, rotation, position).
	modelScale2 = glm::mat4(1.0f);
	modelRotate2 = glm::mat4(1.0f);
	modelTranslate2 = glm::mat4(1.0f);

	// scale background and translate background.
	b_scaleFactor = { 60.0f, 50.0f, 1.0f };
	backgroundScale = glm::scale(backgroundScale, glm::vec3(b_scaleFactor));
	backgroundTranslate = glm::translate(backgroundTranslate, glm::vec3(0.0f, 0.0f, -2.0f));

	// initially scale and place cube model in the scene.
	modelScale = glm::scale(modelScale, glm::vec3(0.7f, 0.7f, 0.7f));
	modelTranslate = glm::translate(modelTranslate, glm::vec3(1.0f, 0.0f, -1.0f));

	// initially scale and place sphere model in the scene.
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
		// tells program to bind the shader.
		glBindTexture(GL_TEXTURE_2D, texArray[0].texture);
		// tells program to render to shader.
		background.render();
		//==================================================================================================================================

		//==================================================================================================================================
		// cube model rendering
		//==================================================================================================================================

		////set .obj model
		glUseProgram(model.shaderProgram);
		//lighting uniforms
		//get and set light colour and position uniform
		lightColLocation = glGetUniformLocation(model.shaderProgram, "lightCol");
		glUniform3fv(lightColLocation, 1, glm::value_ptr(lightColour));
		lightPositionLocation = glGetUniformLocation(model.shaderProgram, "lightPos");
		glUniform3fv(lightPositionLocation, 1, glm::value_ptr(lightPosition));
		//rotation
		modelRotate = glm::rotate(modelRotate, (float)elapsedTime / 2000, glm::vec3(0.0f, 1.0f, 0.0f));
		importModelLocation = glGetUniformLocation(model.shaderProgram, "uModel");
		glUniformMatrix4fv(importModelLocation, 1, GL_FALSE, glm::value_ptr(modelTranslate*modelRotate*modelScale));
		importViewLocation = glGetUniformLocation(model.shaderProgram, "uView");
		glUniformMatrix4fv(importViewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		importProjectionLocation = glGetUniformLocation(model.shaderProgram, "uProjection");
		glUniformMatrix4fv(importProjectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		//set the normal matrix to send to the vertex shader
		//Light calculations take place in model-view space
		//So we calculate the normal matrix in that space
		normalMatrix = glm::transpose(glm::inverse(modelTranslate*modelRotate*modelScale * viewMatrix));
		//set the normalMatrix in the shaders
		glUseProgram(model.shaderProgram);
		normalMatrixLocation = glGetUniformLocation(model.shaderProgram, "uNormalMatrix");
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glBindTexture(GL_TEXTURE_2D, texArray[1].texture);
		model.render();
		//==================================================================================================================================

		//==================================================================================================================================
		// sphere model rendering
		//==================================================================================================================================

		// set .obj model
		glUseProgram(model.shaderProgram);

		// get and set light colour.
		lightColLocation = glGetUniformLocation(model.shaderProgram, "lightCol");
		glUniform3fv(lightColLocation, 1, glm::value_ptr(lightColour));

		// get and set light position.
		lightPositionLocation = glGetUniformLocation(model.shaderProgram, "lightPos");
		glUniform3fv(lightPositionLocation, 1, glm::value_ptr(lightPosition));

		//rotation
		modelRotate = glm::rotate(modelRotate, (float)elapsedTime / 2000, glm::vec3(0.0f, 1.0f, 0.0f));
		importModelLocation = glGetUniformLocation(model.shaderProgram, "uModel");
		glUniformMatrix4fv(importModelLocation, 1, GL_FALSE, glm::value_ptr(modelTranslate2*modelRotate*modelScale));
		importViewLocation = glGetUniformLocation(model.shaderProgram, "uView");
		glUniformMatrix4fv(importViewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		importProjectionLocation = glGetUniformLocation(model.shaderProgram, "uProjection");
		glUniformMatrix4fv(importProjectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		//set the normal matrix to send to the vertex shader
		//Light calculations take place in model-view space
		//So we calculate the normal matrix in that space
		normalMatrix = glm::transpose(glm::inverse(modelTranslate2*modelRotate*modelScale * viewMatrix));
		//set the normalMatrix in the shaders
		glUseProgram(model.shaderProgram);
		normalMatrixLocation = glGetUniformLocation(model.shaderProgram, "uNormalMatrix");
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix2));
		glBindTexture(GL_TEXTURE_2D, texArray[2].texture);
		model2.render();
		//==================================================================================================================================

		// update the window after everything has been rendered in the loop.
		SDL_GL_SwapWindow(sdl.win);

	}//end loop

	// delete context from memory. (cleanup).
	SDL_GL_DeleteContext(context);

	// quit SDL.
	SDL_Quit();
	return 0;
}
//==========================================================================================================================================
// Handle User Input Function
//==========================================================================================================================================
void handleInput()
{
	//*****************************
		//SDL handled input
		//Any input to the program is done here

	if (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			windowOpen = false;
		}
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
		
		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
			
			case SDLK_UP:
				modelTranslate2 = glm::translate(modelTranslate2, glm::vec3((float)cos(angle)*0.02f, (float)sin(angle)*0.02f, 0.0f));
				break;
			case SDLK_LEFT:
				angle += glm::radians(45.0f);
				rotate = glm::rotate(rotate,glm::radians(10.0f), glm::vec3(0, 0, 1));
				break;
			case SDLK_RIGHT:
				angle -= glm::radians(45.0f);
				rotate = glm::rotate(rotate, glm::radians(-10.0f) , glm::vec3(0, 0, 1));
				break;

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
				// move light left.
			case SDLK_f:
				lightPosition.x -= 0.1f;
				break;
				// move light right.
			case SDLK_h:
				lightPosition.x += 0.1f;
				break;
				// move light up.
			case SDLK_g:
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
#endif
#endif
#endif
#endif
