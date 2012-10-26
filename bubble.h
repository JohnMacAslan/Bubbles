#define _USE_MATH_DEFINES
#include <SFML/Window.hpp>
#include <stdio.h>
#include <math.h>

#define RADIUS 5
#define TWO_PI 2*M_PI
#define PI_16TH M_PI/16

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
	//z=-50;
	//x=0;
	//y=0;
	//xRate=0;
	//yRate=0;
	//zRate=0;
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
			for(i = 0.0; i < TWO_PI; i += PI_16TH) {
				glNormal3f(cos(xRate*Clock.GetElapsedTime()) + radiusAtHeight * cos(i), radiusAtHeight * sin(i), height + sin(zRate*Clock.GetElapsedTime()));
				glVertex3d(x + cos(xRate*Clock.GetElapsedTime()) + radiusAtHeight * cos(i), y + radiusAtHeight * sin(i), z + height + sin(zRate*Clock.GetElapsedTime()));
				glNormal3f(cos(xRate*Clock.GetElapsedTime()) + radiusAtNextHeight * cos(i), radiusAtNextHeight * sin(i), (height + heightIncrement) + sin(zRate*Clock.GetElapsedTime()));
				glVertex3d(x + cos(xRate*Clock.GetElapsedTime()) + radiusAtNextHeight * cos(i), y + radiusAtNextHeight * sin(i), z + (height + heightIncrement) + sin(zRate*Clock.GetElapsedTime()));
			}
		}
	}
	glEnd();
}