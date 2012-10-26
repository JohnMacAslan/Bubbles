#define _USE_MATH_DEFINES
#include "glew.h"
#include <SFML/Window.hpp>
#include <stdio.h>
#include <math.h>
#include "bubble.h"
#include "ShaderManager.h"

#define BUBBLE_MODE 100
#define RES_WIDTH 1024
#define RES_HEIGHT 768
#define NUM_BUBBLES 1000

bool GL20Support;
float lightRadius = 3;
float lightHeight = 3;
float lightPos[4] = {5,5,-5,-1};

float motionTime = .0f;
int lastPos[2] = {0,0};
int buttonDown[3] = {0,0};
int xsize, ysize;

int mode;
Bubble bubbles[NUM_BUBBLES];

/* This function initialize the graphics state
*/
void gfxinit()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, ((double) RES_WIDTH) / ((double) RES_HEIGHT), 1.0, 200.0);
}

class GLBox
{
public:
	GLBox()
	{
		App = new sf::Window(sf::VideoMode(RES_WIDTH, RES_HEIGHT, 32), "Bubbles!");

		// Setting up shaders
		FILE * logFile;
		logFile = fopen("log text", "wb");
		if(NULL == logFile)
		{
			printf("Unable to open log file. Exiting...\n");
			exit(2);
		}
		__glewInit(logFile);

		ShaderManager shaders = ShaderManager(logFile);
		char const * bubbleDrawVert = "BubbleShaders/bubbleRender3dModel.vert";
		char const * bubbleDrawFrag = "BubbleShaders/bubbleRender3dModel.frag";
		bubbleShaderProg = shaders.buildShaderProgram(&bubbleDrawVert, &bubbleDrawFrag, 1, 1);

		Clock = sf::Clock();
		mode = BUBBLE_MODE;
		gfxinit();

		while (App->IsOpened())
		{
			App->SetActive();
			handleEvents();
			setShader();
			setShaderVariables(bubbleShaderProg);
			display();
			App->Display();
		}

		fclose(logFile);
	}

	~GLBox()
	{
	}

private:
	GLuint bubbleShaderProg;
	sf::Window *App;
	sf::Clock Clock, motionClock;
	float _near, _far, fov, cursorScrollAmount, center[3], cameraPos[3], lookAtPos[3], cameraUp[3], lightPos[3], 
		lastClickPos[2], lastFrameDragPos[2], cursorDragAmount[2], cursorAbsolutePos[2], biggestSize, timeSinceMotion;
	bool mouseButtonDown;

	void handleEvents()
	{
		const sf::Input& Input = App->GetInput();
		bool shiftDown = Input.IsKeyDown(sf::Key::LShift) || Input.IsKeyDown(sf::Key::RShift);
		sf::Event Event;
		while (App->GetEvent(Event))
		{
			// Close window : exit
			if (Event.Type == sf::Event::Closed)
				App->Close();

			// Escape key : exit
			if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))
				App->Close();

			if (Event.Type == sf::Event::Resized)
			{
				glViewport(0, 0, Event.Size.Width, Event.Size.Height);
			}
		}
	}

	void setShader() {
		if(GL20Support){
			switch (mode) {
				case BUBBLE_MODE:
					glUseProgram(bubbleShaderProg);
					break;
				default:
					glUseProgram(bubbleShaderProg);
			}
		} else {
			switch (mode) {
				case BUBBLE_MODE:
					glUseProgramObjectARB(bubbleShaderProg);
					break;
				default:
					glUseProgramObjectARB(bubbleShaderProg);
			}
		}
	}

	void display()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		setShader();
		setShaderVariables(bubbleShaderProg);
		switch (mode) {
			case BUBBLE_MODE:
				glClearColor(0.0f, 0.15f, 0.25f, 0.0f);
				for(int i = 0; i < NUM_BUBBLES; i++) {
					bubbles[i].draw();
				}
				break;
		}
		glFlush();
	}

	void setShaderVariables(GLuint bubbleShaderProg)
	{
		GLfloat projMatrix[16];
		GLfloat viewMatrix[16];
		glGetFloatv(GL_PROJECTION_MATRIX, projMatrix);
		glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);

		if(GL20Support)
		{
			glUniform1f(glGetUniformLocation(bubbleShaderProg, "elapsedTime"), motionClock.GetElapsedTime());
			glUniform1f(glGetUniformLocation(bubbleShaderProg, "near"), _near);
			glUniform1f(glGetUniformLocation(bubbleShaderProg, "far"), _far);
			glUniform1f(glGetUniformLocation(bubbleShaderProg, "fov"), fov);
			glUniform1f(glGetUniformLocation(bubbleShaderProg, "cursorScrollAmount"), cursorScrollAmount);
			glUniform2f(glGetUniformLocation(bubbleShaderProg, "resolution"), RES_WIDTH, RES_HEIGHT);
			glUniform3f(glGetUniformLocation(bubbleShaderProg, "modelCenter"),  center[0], center[1], center[2]);
			glUniform3f(glGetUniformLocation(bubbleShaderProg, "cameraPos"),  cameraPos[0], cameraPos[1], cameraPos[2]);
			glUniform3f(glGetUniformLocation(bubbleShaderProg, "lookAtPos"),  lookAtPos[0], lookAtPos[1], lookAtPos[2]);
			glUniform3f(glGetUniformLocation(bubbleShaderProg, "cameraUp"),  cameraUp[0], cameraUp[1], cameraUp[2]);
			glUniformMatrix4fv(glGetUniformLocation(bubbleShaderProg, "projMatrix"), 1, false, projMatrix);
			glUniformMatrix4fv(glGetUniformLocation(bubbleShaderProg, "viewMatrix"), 1, false, viewMatrix);
			glUniform2f(glGetUniformLocation(bubbleShaderProg, "lightPos"), lightPos[0], lightPos[1]);
			glUniform2f(glGetUniformLocation(bubbleShaderProg, "cursorAbsolutePos"), cursorAbsolutePos[0], cursorAbsolutePos[1]);
			glUniform2f(glGetUniformLocation(bubbleShaderProg, "cursorDragAmount"), cursorDragAmount[0], cursorDragAmount[1]);
			glUniform2f(glGetUniformLocation(bubbleShaderProg, "lastClickPos"), lastClickPos[0], lastClickPos[1]);
			glUniform2f(glGetUniformLocation(bubbleShaderProg, "lastFrameDragPos"), lastFrameDragPos[0], lastFrameDragPos[1]);
			glUniform1i(glGetUniformLocation(bubbleShaderProg, "mouseButtonDown"), mouseButtonDown);
		} else {
			glUniform1fARB(glGetUniformLocationARB(bubbleShaderProg, "elapsedTime"), Clock.GetElapsedTime());
			glUniform1fARB(glGetUniformLocationARB(bubbleShaderProg, "near"), _near);
			glUniform1fARB(glGetUniformLocationARB(bubbleShaderProg, "far"), _far);
			glUniform1fARB(glGetUniformLocationARB(bubbleShaderProg, "fov"), fov);
			glUniform1fARB(glGetUniformLocationARB(bubbleShaderProg, "cursorScrollAmount"), cursorScrollAmount);
			glUniform2fARB(glGetUniformLocationARB(bubbleShaderProg, "resolution"), RES_WIDTH, RES_HEIGHT);
			glUniform3fARB(glGetUniformLocationARB(bubbleShaderProg, "modelCenter"),  center[0], center[1], center[2]);
			glUniform3fARB(glGetUniformLocationARB(bubbleShaderProg, "cameraPos"),  cameraPos[0], cameraPos[1], cameraPos[2]);
			glUniform3fARB(glGetUniformLocationARB(bubbleShaderProg, "lookAtPos"),  lookAtPos[0], lookAtPos[1], lookAtPos[2]);
			glUniform3fARB(glGetUniformLocationARB(bubbleShaderProg, "cameraUp"),  cameraUp[0], cameraUp[1], cameraUp[2]);
			glUniformMatrix4fvARB(glGetUniformLocationARB(bubbleShaderProg, "projMatrix"), 1, false, projMatrix);
			glUniformMatrix4fvARB(glGetUniformLocationARB(bubbleShaderProg, "viewMatrix"), 1, false, viewMatrix);
			glUniform2fARB(glGetUniformLocationARB(bubbleShaderProg, "cursorAbsolutePos"), cursorAbsolutePos[0], cursorAbsolutePos[1]);
			glUniform2fARB(glGetUniformLocationARB(bubbleShaderProg, "cursorDragAmount"), cursorDragAmount[0], cursorDragAmount[1]);
			glUniform2fARB(glGetUniformLocationARB(bubbleShaderProg, "lastClickPos"), lastClickPos[0], lastClickPos[1]);
			glUniform2fARB(glGetUniformLocationARB(bubbleShaderProg, "lastFrameDragPos"), lastFrameDragPos[0], lastFrameDragPos[1]);
			glUniform1iARB(glGetUniformLocationARB(bubbleShaderProg, "mouseButtonDown"), mouseButtonDown);

		}
	}

	void __glewInit(FILE * logFile) const
	{
		GL20Support = false;
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			fprintf(logFile, "Error: %s\n", glewGetErrorString(err));
		}
		else
		{
			printf("GLEW init finished...\n");
			fprintf(logFile, "GLEW init finished...\n");
			if( __GLEW_VERSION_2_0 )
			{
				printf("OpenGL 2.0 is supported. Shaders should run correctly.\n");
				fprintf(logFile, "OpenGL 2.0 is supported. Shaders should run correctly.\n");
				GL20Support = true;
			}
			else
			{
				printf("OpenGL 2.0 is NOT enabled. The program may not work correctly.\n");
				fprintf(logFile, "OpenGL 2.0 is NOT enabled. The program may not work correctly.\n");
			}

			if( GLEW_ARB_vertex_program )
			{
				printf("ARB vertex programs supported.\n");
				fprintf(logFile, "ARB vertex programs supported.\n");
			}
			else
			{
				printf("ARB vertex programs NOT supported. The program may not work correctly.\n");
				fprintf(logFile, "ARB vertex programs NOT supported. The program may not work correctly.\n");
			}
			if( GLEW_ARB_fragment_program )
			{
				printf("ARB fragment programs supported.\n");
				fprintf(logFile, "ARB fragment programs supported.\n");
			}
			else
			{
				printf("ARB fragment programs NOT supported. The program may not work correctly.\n");
				fprintf(logFile, "ARB fragment programs NOT supported. The program may not work correctly.\n");
			}
		}
	}
};

int main()
{
	GLBox prog;
	return EXIT_SUCCESS;
}