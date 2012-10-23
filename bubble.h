#define _USE_MATH_DEFINES
#include <SFML/Window.hpp>
#include <stdio.h>
#include <math.h>

#define RADIUS 5
#define TWO_PI 2*M_PI
#define PI_32ND M_PI/32

struct Bubble {
	double x,y,z;
	Bubble(double, double, double);
	void draw();
};

Bubble::Bubble(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

/* Draws a bubble
*/
void Bubble::draw(){
	double i;
	double height;
	double heightIncrement = RADIUS / 16.0;
	double radiusAtHeight;
	double radiusAtNextHeight;

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_TRIANGLE_STRIP);
	{
		for(height = -RADIUS; height < RADIUS; height += heightIncrement) {
			radiusAtHeight = sqrt(RADIUS*RADIUS - height*height);
			radiusAtNextHeight = sqrt(RADIUS*RADIUS - (height + heightIncrement)*(height + heightIncrement));
			for(i = 0.0; i < TWO_PI; i += PI_32ND) {
				glVertex3d(x + radiusAtHeight * cos(i), y + radiusAtHeight * sin(i), height + z);
				glVertex3d(x + radiusAtNextHeight * cos(i), y + radiusAtNextHeight * sin(i), (height + heightIncrement) + z);
			}
		}
	}
	glEnd();
}