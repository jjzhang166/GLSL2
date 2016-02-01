#version 400

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;

uniform mat4 uWorld;
out vec2 TexCoord0;

void main()
{
	TexCoord0 = TexCoord;
	//Color = (clamp(Position, 0.0, 1.0));
	gl_Position = uWorld * vec4(Position, 1.0);
}
	