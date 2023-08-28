// include C++ headers
#define _USE_MATH_DEFINES
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
#define SLICES 32
float gScaleFactor = static_cast<float>(gWindowHeight) / gWindowWidth;

// struct for size computation
struct VertexColor
{
	GLfloat position[3];
	GLfloat colour[3];
};

// defines initialiseVertices
// global vertices vector
void initialiseVertices();
void initialiseWheels();
std::vector<GLfloat> vertices;

// scene content
ShaderProgram gShader;	// shader program object
GLuint gVBO = 0;		// vertex buffer object identifier
GLuint gVAO = 0;		// vertex array object identifier



// generate vertices for a circle based on a radius and number of slices
void generate_circle(const float radius, const float scale_factor, std::vector<GLfloat>& vertices, float centreX, float centreY, bool isRim)
{
	float slice_angle = M_PI * 2.0f / SLICES;	// angle of each slice
	float angle = 0;			// angle used to generate x and y coordinates
	float x, y, z = 0.0f;		// (x, y, z) coordinates

	// these variables are for holding the slices that we want to get the rim reflect effect
	// this decides which slices to apply certain colour
	int rimReflectMinPos = 4, rimReflectMaxPos = 5; // can change this to increase number of slices affected
	int totalSlices = SLICES + 1;
	int offset = 1; // can change this to move one of the reflections

	// generate vertex coordinates for a circle
	for (int i = 0; i <= SLICES; i++)
	{
		x = centreX + radius * cos(angle) * scale_factor;
		y = centreY + radius * sin(angle);

		// pushes vertex co-ords to vertices
		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(z);

		// if statement to check if it's the rim or tyre
		// it then pushes the colour information to the vertices vector
		if (isRim) {
			if ((i >= rimReflectMinPos- offset && i <= rimReflectMaxPos- offset) || (i >= (totalSlices + rimReflectMinPos)/2 % totalSlices && i <= (totalSlices + rimReflectMaxPos)/2 % totalSlices)) {
				vertices.push_back(0.7f);
				vertices.push_back(0.7f);
				vertices.push_back(0.7f);

			}
			else {
				vertices.push_back(0.5f);
				vertices.push_back(0.5f);
				vertices.push_back(0.5f);

			}

		}
		else {
			vertices.push_back(0.2f);
			vertices.push_back(0.2f);
			vertices.push_back(0.2f);
		}

		// update to next angle
		angle += slice_angle;
	}
}

// function initialise scene and render settings
static void init(GLFWwindow* window)
{
	// set the color the color buffer should be cleared to
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	gShader.compileAndLink("truck.vert", "truck.frag");

	initialiseVertices(); // initialises truck body vertices



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
	// not for any particular reason
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 6); // front cabin
	glDrawArrays(GL_TRIANGLE_FAN, 6, 4); // i used a fan here just to try out different types
	glDrawArrays(GL_TRIANGLE_STRIP, 10, 6); // back tray
	glDrawArrays(GL_TRIANGLE_STRIP, 16, 4); // truck base
	glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 24, SLICES + 2); // front tyre
	glDrawArrays(GL_TRIANGLE_FAN, 58, SLICES + 2); // front rim
	glDrawArrays(GL_TRIANGLE_FAN, 92, SLICES + 2); // back tyre
	glDrawArrays(GL_TRIANGLE_FAN, 126, SLICES + 2); // back rim

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


// moved vertices to its own function to clean up the init function
void initialiseVertices() {

	vertices = {

		// Truck front cabin - 6 vertices
		//
		// bottom right
		-0.13f, -0.5f, 0.0f, // x same as top right
		1.0f, 0.0f, 0.0f,


		// bottom left
		-0.4, -0.5f, 0.0f, // y same as bottom right, x same as
		1.0f, 0.0f, 0.0f,

		// middle right
		-0.13f, -0.275, 0.0f,
		1.0f, 0.0f, 0.0f,

		// middle left
		-0.4, -0.2, 0.0f, // x same as bottom left
		0.0f, 1.0f, 0.0f,

		// top right vertex
		-0.13f, -0.05f, 0.0f, // vertex
		0.0f, 1.0f, 0.0f, // colour

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
		// top left
		-0.05f, -0.08f, 0.0f,
		0.0f, 0.0f, 1.0f,

		// top right
		0.4f, -0.08f, 0.0f, // y same as top left
		0.0f, 0.0f, 1.0f,
		
		// middle left
		-0.13f, -0.275, 0.0f,
		0.0f, 0.0f, 1.0f,

		// middle right
		0.48f, -0.275f, 0.0f, // y same as middle left
		0.0f, 0.5f, 0.5f,

		// bottom left
		-0.05f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.5f,

		// bottom right
		0.4f, -0.5f, 0.0f, // x same as top right
		0.0f, 0.5f, 0.5f,



		// Truck base - 4 vertices
		// top left
		-0.4, -0.5, -0.0f,
		0.8f, 0.8f, 0.8f,

		// top right
		0.4f, -0.5, 0.0f, // y same as top left
		0.8f, 0.8f, 0.8f,

		// bottom left
		-0.4, -0.55, 0.0f, // x same as top left
		0.2f, 0.2f, 0.2f,

		// bottom right
		0.4f, -0.55, 0.0f, // x same as top right, y same as bottom left
		0.2f, 0.2f, 0.2f,

		// ground - 4 vertices
		// can use co-ords of y of wheel + radius of tyre to get point of contact
		// top left
		-1.0f, -0.625f, 0.0f,
		0.0f, 0.8f, 0.2f,

		// top right
		1.0f, -0.625, 0.0f,
		0.0f, 0.6f, 0.4f,

		// bottom left
		-1.0f, -1.0f, 0.0f,
		0.0f, 0.2f, 0.0f,

		// bottom right
		1.0f, -1.0f, 0.0f,
		0.0f, 0.2f, 0.0f,



	};

	initialiseWheels();



}


void initialiseWheels() {

	float x, y, z = 0.0f; // to hold centre pos of circle
	// wheels
	// 
	// front tyre
	// centre vertex
	x = -0.265f;
	y = -0.525f;
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);
	// first colour
	vertices.push_back(0.6f);
	vertices.push_back(0.6f);
	vertices.push_back(0.6f);

	generate_circle(0.15f, gScaleFactor, vertices, x, y, false); // generates front tyre

	// front rim
	// centre vertex
	x = -0.265f;
	y = -0.525f;
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);
	// first colour
	vertices.push_back(0.9f);
	vertices.push_back(0.9f);
	vertices.push_back(0.9f);

	generate_circle(0.1f, gScaleFactor, vertices, x, y, true); // generates front rim

	// back tyre
	//
	// centre vertex
	x = 0.275f;
	y = -0.525f;
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);
	// first colour
	vertices.push_back(0.6f);
	vertices.push_back(0.6f);
	vertices.push_back(0.6f);

	generate_circle(0.15f, gScaleFactor, vertices, x, y, false); // generates back tyre

	// back rim 
	//
	// centre vertex
	x = 0.275f;
	y = -0.525f;
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);
	// first colour
	vertices.push_back(0.9f);
	vertices.push_back(0.9f);
	vertices.push_back(0.9f);

	generate_circle(0.1f, gScaleFactor, vertices, x, y, true); // generates back tyre

}