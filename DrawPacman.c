/* the spline control points */

#include <math.h>

#include <ctype.h>
#include <stdlib.h>

/* GLUT headers */
//#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
//OSX import
#ifdef OSX
    #include <GLUT/glut.h>
#else
    #include "/usr/include/GL/glut.h"
#endif


#include "includes.h"
#include "DrawPacman.h"

struct Point g_ControlPointsPacman[ControlPointCount];
struct Point *g_SelectedPoint;



/************ INITIALISATION ***************/

void InitialisePacmanControlPoints(void)
{
	/* some pleasant default values for our spline's control points */
	g_SelectedPoint = NULL;

	g_ControlPointsPacman[0].m_X = .00;
	g_ControlPointsPacman[0].m_Y = .00;
	
	g_ControlPointsPacman[1].m_X = .250;
	g_ControlPointsPacman[1].m_Y = .0;
	
	g_ControlPointsPacman[2].m_X = .250;
	g_ControlPointsPacman[2].m_Y = .200;



	g_ControlPointsPacman[3].m_X = .250;
	g_ControlPointsPacman[3].m_Y = .400;


	g_ControlPointsPacman[4].m_X = .00;
	g_ControlPointsPacman[4].m_Y = .400;

}


/* The bernstein polynomials - see Foley p 338 */
float Bernstein1(float x)
{
	return pow((1-x),4);
}
float Bernstein2(float x)
{
	return 4*x*pow((1-x),3);
}
float Bernstein3(float x)
{
	return 6*pow(x,2)*pow((1-x),2);
}
float Bernstein4(float x)
{
	return 4*(1-x)*pow(x,3);
}
float Bernstein5(float x)
{
	return pow(x,4);
}

void DrawPacman(void)
{
	/*
	 This function is where we actually draw our spline
	 */
	int i;
	int g_SegmentCount= 45;
	float x[45], y[45];
	int degree;
	float gradient;
	float mag;

	/* tell OpenGL to start drawing a strip of lines */
	for (i = 0; i < g_SegmentCount; i++)
	{
		x[i] = 	Bernstein1((float)i/g_SegmentCount) * g_ControlPointsPacman[0].m_X +
			Bernstein2((float)i/g_SegmentCount) * g_ControlPointsPacman[1].m_X +
			Bernstein3((float)i/g_SegmentCount) * g_ControlPointsPacman[2].m_X +
			Bernstein4((float)i/g_SegmentCount) * g_ControlPointsPacman[3].m_X +
			Bernstein5((float)i/g_SegmentCount) * g_ControlPointsPacman[4].m_X;
		y[i] = 	Bernstein1((float)i/g_SegmentCount) * g_ControlPointsPacman[0].m_Y +
			Bernstein2((float)i/g_SegmentCount) * g_ControlPointsPacman[1].m_Y +
			Bernstein3((float)i/g_SegmentCount) * g_ControlPointsPacman[2].m_Y +
			Bernstein4((float)i/g_SegmentCount) * g_ControlPointsPacman[3].m_Y +
			Bernstein5((float)i/g_SegmentCount) * g_ControlPointsPacman[4].m_Y;
	}
	/* set an appropriate colour for the line */
	glBegin(GL_TRIANGLE_STRIP);
	for (degree = 0;degree<360;degree+=3)
		{
		for (i = 0; i < g_SegmentCount; i++){
			
			if((degree>270))
			{

				glColor3f(0.0f,0.0f,0.0f);
				if(i==0)
				{
					glNormal3f(0.0f,1.0f,0.0f);
				}
				else
				{
					gradient= (x[i-1]-x[i])/(y[i]-y[i-1]);
					mag = sqrt(1+pow(gradient,2));
					glNormal3f(cos((degree)/180.0*Pi)/mag,sin((degree)/180.0*Pi)/mag,gradient/mag);
				}
				glVertex3f(x[i]*cos(degree/180.0*Pi),x[i]*sin(degree/180.0*Pi),y[i]);
				glColor3f(1.0f,1.0f,0.0f);
				if(i==0)
				{
					glNormal3f(0.0f,1.0f,0.0f);
				}
				else
				{
					glNormal3f(cos((degree+3)/180.0*Pi)/mag,sin((degree+3)/180.0*Pi)/mag,gradient/mag);
				}
				glVertex3f(x[i]*cos((degree+3)/180.0*Pi),x[i]*sin((degree+3)/180.0*Pi),y[i]);
			} else {
				
				glColor3f(1.0f,1.0f,0.0f);
				if(i==0)
				{
					glNormal3f(0.0f,1.0f,0.0f);
				}
				else
				{
					gradient= (x[i-1]-x[i])/(y[i]-y[i-1]);
					mag = sqrt(1+pow(gradient,2));
					glNormal3f(cos((degree)/180.0*Pi)/mag,sin((degree)/180.0*Pi)/mag,gradient/mag);
				}
				glVertex3f(x[i]*cos(degree/180.0*Pi),x[i]*sin(degree/180.0*Pi),y[i]);
				if(i==0)
				{
					glNormal3f(0.0f,1.0f,0.0f);
				}
				else
				{
					glNormal3f(cos((degree+3)/180.0*Pi)/mag,sin((degree+3)/180.0*Pi)/mag,gradient/mag);
				}
				glVertex3f(x[i]*cos((degree+3)/180.0*Pi),x[i]*sin((degree+3)/180.0*Pi),y[i]);
			}
		
		}
	}
	glEnd();
	/* finish drawing the lines */
}
