
#version 330 core
out vec4 color;

uniform vec4 light;

void main()
{
    //color = vec4(1.0f); // Set alle 4 vector values to 1.0f
	color = light;
}