#define _USE_MATH_DEFINES
#include <SFML/Window.hpp>
#include <stdio.h>
#include <math.h>

#define RADIUS 5
#define TWO_PI 2*M_PI
#define PI_32ND M_PI/32

struct Bubble {
	sf::Clock Clock;
	double x,y,z,xRate,yRate,zRate;
	Bubble();
	void setValues();
	void draw();
};

void Bubble::setValues(){
	z = -75 - rand()%100;
	x = (-z/2.0) - rand()%(-(int)z);
	y = z;
	xRate = 10+rand()%10;
	yRate = 200/(-z);
	zRate = 10+rand()%10;
}

Bubble::Bubble()
{
	setValues();
}

/* Draws a bubble
*/
void Bubble::draw(){
	double i;
	double height;
	double heightIncrement = RADIUS / 16.0;
	double radiusAtHeight;
	double radiusAtNextHeight;

	y+=yRate;
	if(y > -z) {
		setValues();
	}

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_TRIANGLE_STRIP);
	{
		for(height = -RADIUS; height < RADIUS; height += heightIncrement) {
			radiusAtHeight = sqrt(RADIUS*RADIUS - height*height);
			radiusAtNextHeight = sqrt(RADIUS*RADIUS - (height + heightIncrement)*(height + heightIncrement));
			for(i = 0.0; i < TWO_PI; i += PI_32ND) {
				glVertex3d(x + cos(xRate*Clock.GetElapsedTime()) + radiusAtHeight * cos(i), y + radiusAtHeight * sin(i), height + z + sin(zRate*Clock.GetElapsedTime()));
				glVertex3d(x + cos(xRate*Clock.GetElapsedTime()) + radiusAtNextHeight * cos(i), y + radiusAtNextHeight * sin(i), (height + heightIncrement) + z + sin(zRate*Clock.GetElapsedTime()));
			}
		}
	}
	glEnd();
}