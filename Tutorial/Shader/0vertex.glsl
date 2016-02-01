#version 400

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 VertexColor;

out vec3 Color;

uniform mat4 uWorld;

void main()
{
	Color = VertexColor;
	gl_Position = uWorld * vec4(Position*0.5, 1.0);
}
