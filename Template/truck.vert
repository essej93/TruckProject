#version 330 core

// input data
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;

// output data
out vec3 vColor;

void main()
{
	// set vertex position
    gl_Position = vec4(aPosition, 1.0f);

	// set vertex shader output color 
	// will be interpolated for each fragment
	vColor = aColor;
}
