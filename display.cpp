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

bool GL20Support;
float lightRadius = 3;
float lightHeight = 3;
float lightPos[4] = {5,5,-5,-1};

float motionTime = .0f;
int lastPos[2] = {0,0};
int buttonDown[3] = {0,0};
int spin=FALSE;                    // are we spinning?
int xsize, ysize;

int mode;
Bubble firstBubble(-15.0, -30.0, -150.0);
Bubble secondBubble(6.0, 8.0, -90.0);

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	switch (mode) {
		case BUBBLE_MODE:
			glClearColor(0.05f, 0.2f, 0.4f, 0.0f);
			firstBubble.draw();
			secondBubble.draw();
			break;
	}
}

/* This function initialize the graphics state
*/
void gfxinit()
{
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, ((double) RES_WIDTH) / ((double) RES_HEIGHT), 1.0, 200.0);
}

class GLBox
{
public:
	GLBox()
	{
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
		shaders.GL20Support = GL20Support;
		char const * bubbleDrawVert = "BubbleShaders/bubbleRender3dModel.vert";
		char const * bubbleDrawFrag = "BubbleShaders/bubbleRender3dModel.frag";
		bubbleShaderProg = shaders.buildShaderProgram(&bubbleDrawVert, &bubbleDrawFrag, 1, 1);

		App = new sf::Window(sf::VideoMode(RES_WIDTH, RES_HEIGHT, 32), "Bubbles!");
		Clock = sf::Clock();
		mode = BUBBLE_MODE;
		gfxinit();

		while (App->IsOpened())
		{
			App->SetActive();
			handleEvents();
			setShader();
			display();
			App->Display();
		}

		fclose(logFile);
	}

	~GLBox()
	{
	}

private:
	Gluint bubbleShaderProg;
	sf::Window *App;
	sf::Clock Clock, motionClock;
	float _near, _far, fov, curserScrollAmount, center[3], cameraPos[3], lookAtPos[3], cameraUp[3], lightPos[3], 
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

			if (Event.Type == sf::Event::MouseButtonPressed)
			{
				lastPos[0] = Event.MouseButton.X;
				lastPos[1] = Event.MouseButton.Y;
				if(Event.MouseButton.Button == sf::Mouse::Left && !shiftDown)
				{
					buttonDown[0] = 1;
					spin = FALSE;
				}
				if(Event.MouseButton.Button == sf::Mouse::Right)
					buttonDown[1] = 1;
				if(Event.MouseButton.Button == sf::Mouse::Middle)
					buttonDown[2] = 1;
				if(Event.MouseButton.Button == sf::Mouse::Left && shiftDown)
					buttonDown[2] = 1;
			}
			if (Event.Type == sf::Event::MouseButtonReleased)
			{
				if(Event.MouseButton.Button == sf::Mouse::Left && !shiftDown)
					buttonDown[0] = 0;
				if(Event.MouseButton.Button == sf::Mouse::Right)
					buttonDown[1] = 0;
				if(Event.MouseButton.Button == sf::Mouse::Middle)
					buttonDown[2] = 0;
				if(Event.MouseButton.Button == sf::Mouse::Left && shiftDown)
					buttonDown[2] = 0;
				timeSinceMotion = motionClock.GetElapsedTime();
				float maxTime = 1.0f/(float)TARGET_FPS * TIME_WINDOW;
				if(timeSinceMotion < maxTime)
					spin = TRUE;
			}
			if (Event.Type == sf::Event::MouseMoved && (buttonDown[0] || buttonDown[1] || buttonDown[2]) )
			{
				int x = Event.MouseMove.X;
				int y = Event.MouseMove.Y;
				timeSinceMotion = motionClock.GetElapsedTime();
				motionClock.Reset();
				if(buttonDown[0])
					update_rotate(lastPos[0], lastPos[1], x, y);
				if(buttonDown[1])
					update_trans(lastPos[0], lastPos[1], x, y);
				if(buttonDown[2])
					update_z(lastPos[0], lastPos[1], x, y);

				lastPos[0] = x;
				lastPos[1] = y;
			}
		}
	}

	void setShader() {
		switch (mode) {
			case BUBBLE_MODE:
				glUseProgram(bubbleShaderProgram);
				break;
			default:
				glUseProgram(bubbleShaderProgram);
		}
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