#version 420 core

out vec2 TexPos;

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 texPos;

uniform vec2 camPos;
uniform float zoom;

float invLerp( float a, float b, float v ){
	return (v - a) / (b - a);
}

void main(){
	gl_Position = vec4(zoom * (aPos + vec2(camPos.x, -camPos.y) * 2 / zoom), 0.0, 1.0);
	TexPos = texPos;
}
