#define _USE_MATH_DEFINES
#include <SFML/Window.hpp>
#include <stdio.h>
#include <math.h>

#define RADIUS 30
#define TWO_PI 2*M_PI

struct Bubble {
	double x,y,z;
	Bubble(double, double);
	void draw();
};

Bubble::Bubble(double x, double y)
{
	this->x = x;
	this->y = y;
}

/* Draws a bubble
*/
void Bubble::draw(){
	double pi_8th = M_PI / 8;
	double i;

	glBegin(GL_POLYGON);
	{
		for(i = 0.0; i < TWO_PI; i+= pi_8th) {
			glVertex2d(x + RADIUS / .8 + RADIUS * cos(i), y - RADIUS / .8 + RADIUS * sin(i));
		}
	}
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINE_LOOP);
	{
		for(i = 0; i < TWO_PI; i+= pi_8th) {
			glVertex2d(x + RADIUS / .8 + RADIUS * cos(i), y - RADIUS / .8 + RADIUS * sin(i));
		}
	}
	glEnd();
}