#version 420 core

out vec4 FragColor;

in vec2 TexPos;

uniform sampler2D screenshot;
uniform vec2 mousePos;

void main(){
	FragColor = texture(screenshot, TexPos);
	if(distance(gl_FragCoord, vec4(mousePos, 0, 0)) <= 30){
		FragColor = mix(FragColor, vec4(1., 1., 1., 1.), 0.5);
	}
}
