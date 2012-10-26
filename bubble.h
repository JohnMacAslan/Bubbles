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
	float xn,yn,zn;
	float time = Clock.GetElapsedTime();

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
				xn = cos(xRate*time) + radiusAtHeight * cos(i);
				yn = radiusAtHeight * sin(i);
				zn = height + sin(zRate*time);
				glNormal3f(xn, yn, zn);
				glVertex3d(x + xn, y + yn, z + zn);

				xn = cos(xRate*time) + radiusAtNextHeight * cos(i);
				yn = radiusAtNextHeight * sin(i);
				zn = (height + heightIncrement) + sin(zRate*time);
				glNormal3f(xn, yn, zn);
				glVertex3d(x + xn, y + yn, z + zn);
			}
		}
	}
	glEnd();
}