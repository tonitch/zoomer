#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "SDL_events.h"
#include "SDL_mouse.h"
#include "SDL_video.h"
#include "tutils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
SDL_Window *w;
SDL_Renderer *r;

void shaderCheck(GLuint shader, const char* type){
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(status == GL_FALSE){
		char msg[1024];
		glGetShaderInfoLog(shader, 1024, NULL, msg);
		printf("SHADER ERROR (%s): %s\n",type, msg);
	}
}

void programCheck(GLuint prog){
	GLint status;
	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	if(status == GL_FALSE){
		char msg[1024];
		glGetProgramInfoLog(prog, 1024, NULL, msg);
		printf("LINKING ERROR: %s\n", msg);
	}
}

int main(int argc, char *argv[])
{
	const char *ProgramName = pullArg(&argc, &argv);

	// TODO <tonitch>: Handle arguments
	const char *c;
	while ( (c = pullArg(&argc, &argv)) )
		printf("%s\n", c);

	//Screenshot
	char *screenFile, *command;
	asprintf(&screenFile, "/tmp/zoomer.png");
	asprintf(&command, "scrot -oF %s", screenFile);
	system(command);

	int width, height, chan;
	stbi_set_flip_vertically_on_load(1);
	unsigned char *data = stbi_load(screenFile, &width, &height, &chan, 0);

	// TODO <tonitch>: screenshot using X11 

	codeChk(SDL_Init(SDL_INIT_VIDEO), "Can't initialize SDL", SDL_GetError);

	w = SDL_CreateWindow(ProgramName, 0, 0, width, height, SDL_WINDOW_OPENGL);
	r = SDL_CreateRenderer(w, -1, SDL_RENDERER_PRESENTVSYNC);
	SDL_ShowCursor(SDL_DISABLE);

	glewInit();

	SDL_GL_SetAttribute(GL_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(GL_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GLContext *glctx = SDL_GL_CreateContext(w);

	GLuint vshader = glCreateShader(GL_VERTEX_SHADER),
		   fshader = glCreateShader(GL_FRAGMENT_SHADER);
	const char *vshadersrc = readFile("vshader.glsl"),
			   *fshadersrc = readFile("fshader.glsl");

	glShaderSource(vshader, 1, &vshadersrc, NULL);
	glShaderSource(fshader, 1, &fshadersrc, NULL);

	glCompileShader(vshader);
	glCompileShader(fshader);
	shaderCheck(vshader, "VERTEX");
	shaderCheck(vshader, "FRAGMENT");

	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	const GLfloat points[] = {
		-1.,  1.,		0., 1.,
		 1.,  1.,		1., 1.,
		 1., -1.,		1., 0.,
		-1., -1.,		0., 0.,
	};
	const GLint tri[] = {
		0, 1, 2,
		0, 2, 3
	};
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4, 0);
	glEnableVertexAttribArray(0);

	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4, (void*)(sizeof(GLfloat)*2));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tri), tri, GL_STATIC_DRAW);

	GLuint pshader = glCreateProgram();
	glAttachShader(pshader, vshader);
	glAttachShader(pshader, fshader);
	glLinkProgram(pshader);
	programCheck(pshader);

	glUseProgram(pshader);

	GLuint screenshotTexture;
	glGenTextures(1, &screenshotTexture);
	glBindTexture(GL_TEXTURE_2D, screenshotTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenshotTexture);

	int shouldClose = 0;
	SDL_Event e;
	float camPos[2] = {0.};
	float mousePos[2] = {0.};
	int zoom = 0;
	int tick = 0;
	while(!shouldClose){
		while(SDL_PollEvent(&e)){
			switch(e.type){
				case SDL_QUIT:
					shouldClose = 1;
					break;
				case SDL_KEYDOWN:
					if(e.key.keysym.sym == SDLK_q)
						shouldClose=1;
					break;
				case SDL_WINDOWEVENT:
					if(e.window.event == SDL_WINDOWEVENT_RESIZED)
						glViewport(0, 0, e.window.data1, e.window.data2);
					break;
				case SDL_MOUSEMOTION:;
					int sw, sh;
					SDL_GetWindowSize(w, &sw, &sh);
					mousePos[0] = e.motion.x;
					mousePos[1] = sh - e.motion.y;
					if(e.motion.state & SDL_BUTTON_LMASK){
						camPos[0] += e.motion.xrel / (float)sw;
						camPos[1] += e.motion.yrel / (float)sh;
					}
					break;
				case SDL_MOUSEWHEEL:
					zoom += e.wheel.y;
					break;
			}
		}
		glClearColor(0., 0., 0., 1.);
		glClear(GL_COLOR_BUFFER_BIT);
		glUniform2f(glGetUniformLocation(pshader, "camPos"), camPos[0], camPos[1]);
		glUniform2f(glGetUniformLocation(pshader, "mousePos"), mousePos[0], mousePos[1]);
		glUniform1i(glGetUniformLocation(pshader, "zoom"), zoom);
		glUniform1i(glGetUniformLocation(pshader, "tick"), tick);
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		/* glDrawArrays(GL_TRIANGLES, 0, 3); */
		SDL_GL_SwapWindow(w);
		tick++;
	}

	SDL_GL_DeleteContext(glctx);
	SDL_Quit();
	return 0;
}
