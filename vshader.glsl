#version 420 core

out vec2 TexPos;

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 texPos;

uniform vec2 camPos;
uniform int zoom;

float invLerp( float a, float b, float v ){
	return (v - a) / (b - a);
}

void main(){
	gl_Position = vec4(
					(aPos + vec2(camPos.x, -camPos.y) * 2) * (invLerp(-10, 10, zoom) * 2),
 					0.0,
					1.0) ;
	TexPos = texPos;
}
