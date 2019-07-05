#version 330 core

//input to be used with our texture to sampel the texture
in vec2 texCoordOut;

//output of the program
out vec4 color;

uniform sampler2D ourTexture;

void main(){
	color = texture(ourTexture, texCoordOut);
	if (color.a == 0.0)
	{
		discard;
	}
}