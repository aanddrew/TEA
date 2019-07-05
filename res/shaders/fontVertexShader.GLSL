#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoordIn;

out vec2 texCoordOut;
uniform sampler2D ourTexture;

void main(){
	gl_Position = vec4(vec3(position,0.0), 1.0);
	texCoordOut = texCoordIn;
}