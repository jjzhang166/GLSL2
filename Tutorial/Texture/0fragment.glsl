#version 400

out vec4 FragColor;
in vec2 TexCoord0;
uniform sampler2D uSampler;

void main()
{
	FragColor = texture2D(uSampler, TexCoord0);
}