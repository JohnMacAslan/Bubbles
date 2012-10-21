#define _USE_MATH_DEFINES
#include <SFML/Window.hpp>
#include <stdio.h>
#include <math.h>
#include "bubble.h"

#define BUBBLE_MODE 100
#define RES_WIDTH 1024
#define RES_HEIGHT 768

int mode;
Bubble firstBubble(RES_WIDTH / 2.0, RES_HEIGHT / 2.0);


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//gluLookAt(0.5, 0.5, 1.2, 0.5, 0.5, 0.0, 0.0, 1.0, 0.0);
	firstBubble.draw();
}

void modeClearColor()
{
	switch (mode) {
		case BUBBLE_MODE:
			glClearColor(0.05f, 0.2f, 0.4f, 0.0f);
			break;
		default:
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	}
}

/* This function initialize the graphics state
 */
void gfxinit()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, RES_WIDTH - 1, 0, RES_HEIGHT - 1);
	//gluPerspective(60, 1.0, 1.0, 50.0);
	modeClearColor();
	//glEnable(GL_DEPTH_TEST);
}

class GLBox
{
public:
	GLBox()
	{
		App = new sf::Window(sf::VideoMode(RES_WIDTH, RES_HEIGHT, 32), "Bubbles!");
		mode = BUBBLE_MODE;
		gfxinit();

		while (App->IsOpened())
		{
			App->SetActive();
			handleEvents();
			display();
			App->Display();
		}
	}
	
	~GLBox()
	{
	}
	
private:
	sf::Window *App;
	
	void handleEvents()
	{
		sf::Event Event;
		while (App->GetEvent(Event))
		{
			switch (Event.Type) {
				// Close window : exit
				case sf::Event::Closed:
					App->Close();
					break;
				// Escape key : exit
				case sf::Event::KeyPressed:
					if (Event.Key.Code == sf::Key::Escape) {
						App->Close();
					}
					break;
				case sf::Event::Resized:
					glViewport(0, 0, Event.Size.Width, Event.Size.Height);
					break;
			}
		}
	}
};


int main()
{
	GLBox prog;
    return EXIT_SUCCESS;
}