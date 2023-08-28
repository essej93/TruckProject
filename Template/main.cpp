// include C++ headers
#include <cstdio>
#include <iostream>
#include <vector>
#include "ShaderProgram.h"
//using namespace std;	// to avoid having to use std::

// include OpenGL related headers
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <AntTweakBar.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

// global variables
// settings
unsigned int gWindowWidth = 800;
unsigned int gWindowHeight = 600;

// wheels/circles info
#define SLICES = 32;
float gScaleFactor = static_cast<float>(gWindowHeight) / gWindowWidth;

// struct for size computation
struct VertexColor
{
	GLfloat position[3];
	GLfloat colour[3];
};

// scene content
ShaderProgram gShader;	// shader program object
GLuint gVBO = 0;		// vertex buffer object identifier
GLuint gVAO = 0;		// vertex array object identifier

// function initialise scene and render settings
static void init(GLFWwindow* window)
{
	// set the color the color buffer should be cleared to
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	gShader.compileAndLink("truck.vert", "truck.frag");

	std::vector<GLfloat> vertices = {
		
		// Truck front cabin - 5 vertices
		// 
		// top right vertex
		-0.13f, -0.05f, 0.0f, // vertex
		0.0f, 1.0f, 0.0f, // colour

		// bottom right
		-0.13f, -0.5f, 0.0f, // x same as top right
		1.0f, 0.0f, 0.0f,

		// bottom left
		-0.4, -0.5f, 0.0f, // y same as bottom right, x same as
		1.0f, 0.0f, 0.0f,

		// middle left
		-0.4, -0.2, 0.0f, // x same as bottom left
		0.0f, 1.0f, 0.0f,

		// top left
		-0.32f, -0.05f, 0.0f, // y same as top right
		0.0f, 1.0f, 0.0f,

		// Truck front window - 4 vertices
		// 
		// bottom left
		-0.38f, -0.2f, 0.0f,
		0.5f, 0.5f, 0.5f,

		// bottom right
		-0.18f, -0.2f, 0.0f, // y same as bottom left
		0.7f, 0.7f, 0.7f,

		// top right
		-0.18f, -0.08f, 0.0f, // x same as bottom right
		0.5f, 0.5f, 0.5f,

		// top left
		-0.31f, -0.08, 0.0f, // y same as top right
		0.7f, 0.7f, 0.7f,

		// Truck tray/bucket - 6 vertices
		// 
		// middle left
		-0.13f, -0.275, 0.0f,
		0.0f, 0.0f, 1.0f,

		// top left
		-0.05f, -0.08f, 0.0f,
		0.0f, 0.0f, 1.0f,

		// top right
		0.4f, -0.08f, 0.0f, // y save as top left
		0.0f, 0.0f, 1.0f,

		// middle right
		0.48f, -0.275f, 0.0f, // y same as middle left
		0.0f, 0.5f, 0.5f,

		// bottom right
		0.4f, -0.5f, 0.0f, // x same as top right
		0.0f, 0.5f, 0.5f,

		// bottom left
		-0.05f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.5f,


	};


	// create VBO and buffer the data
	glGenBuffers(1, &gVBO);					// generate unused VBO identifier
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);	// bind the VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// create VAO, specify VBO data and format of the data
	glGenVertexArrays(1, &gVAO);			// generate unused VAO identifier
	glBindVertexArray(gVAO);				// create VAO
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);	// bind the VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexColor),
		reinterpret_cast<void*>(offsetof(VertexColor, position)));	// specify format of position data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexColor),
		reinterpret_cast<void*>(offsetof(VertexColor, colour)));		// specify format of colour data

	glEnableVertexAttribArray(0);	// enable vertex attributes
	glEnableVertexAttribArray(1);

}

// function to render the scene
static void render_scene()
{
	// clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);



	gShader.use();

	glBindVertexArray(gVAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 5); // front cabin
	glDrawArrays(GL_TRIANGLE_FAN, 5, 4); // front window
	glDrawArrays(GL_TRIANGLE_FAN, 9, 6); // back tray

	// flush the graphics pipeline
	glFlush();
}

// error callback function
static void error_callback(int error, const char* description)
{
	std::cerr << description << std::endl;	// output error description
}

// key callback function
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	// closes if ESC is pressed
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}

}

int main(void)
{
	GLFWwindow* window = nullptr;	// GLFW window handle

	glfwSetErrorCallback(error_callback);	// set GLFW error callback function

	// initialise GLFW
	if (!glfwInit())
	{
		// if failed to initialise GLFW
		exit(EXIT_FAILURE);
	}

	// minimum OpenGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create a window and its OpenGL context
	window = glfwCreateWindow(gWindowWidth, gWindowHeight, "Lab 1", nullptr, nullptr);

	// check if window created successfully
	if (window == nullptr)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);	// set window context as the current context
	glfwSwapInterval(1);			// swap buffer interval

	// initialise GLEW
	if (glewInit() != GLEW_OK)
	{
		// if failed to initialise GLEW
		std::cerr << "GLEW initialisation failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// initialise scene and render settings
	init(window);

	// setting callback functions
	glfwSetKeyCallback(window, key_callback);

	// the rendering loop
	while (!glfwWindowShouldClose(window))
	{
		render_scene();		// render the scene

		glfwSwapBuffers(window);	// swap buffers
		glfwPollEvents();			// poll for events
	}

	// close the window and terminate GLFW
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}