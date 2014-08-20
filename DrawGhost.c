
/* the spline control points */

#include <ctype.h>
#include <stdlib.h>

#include <math.h>

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
#include "DrawGhost.h"
#include "DrawPacman.h"

#define ControlPointCount 5


struct Point (g_ControlPointsGhost)[5];
struct Point *g_SelectedPoint;




/************ INITIALISATION ***************/
// This needs to be run once before the attempt to draw any ghosts.

void InitialiseGhostControlPoints(void)
{
	/* some pleasant default values for our spline's control points */
	g_SelectedPoint = NULL;

	g_ControlPointsGhost[4].m_X = 00;
	g_ControlPointsGhost[4].m_Y = .00;

	g_ControlPointsGhost[3].m_X = .730;
	g_ControlPointsGhost[3].m_Y = .0;

	g_ControlPointsGhost[2].m_X = -.300;
	g_ControlPointsGhost[2].m_Y = .250;

	g_ControlPointsGhost[1].m_X = .50;
	g_ControlPointsGhost[1].m_Y = .375;

	g_ControlPointsGhost[0].m_X = 0.0;
	g_ControlPointsGhost[0].m_Y = 0.400;
}


/************ PER FRAME PROCESSING AND RENDERING ***************/


/* The Ghost_Bernstein polynomials - see Foley p 338 */
float Ghost_Bernstein1(float x)
{
	return pow((1-x),4);
}
float Ghost_Bernstein2(float x)
{
	return 4*x*pow((1-x),3);
}
float Ghost_Bernstein3(float x)
{
	return 6*pow(x,2)*pow((1-x),2);
}
float Ghost_Bernstein4(float x)
{
	return 4*(1-x)*pow(x,3);
}
float Ghost_Bernstein5(float x)
{
	return pow(x,4);
}

void DrawGhost(float distance, float * color, int hunt_flag)
 {
	/*
	 This function is where we actually draw our spline
	 */
	int i;
	int g_SegmentCount;
	float x[45],y[45];
	int degree;	
	float gradient;
	float mag;
	int res;
	distance /=2.0;
	if(distance>= 0.0 && distance <=60.0)
	{
		g_SegmentCount = 45;
		res = 3;
	}
	else if(distance>50.0 && distance <=150.0)
	{
		g_SegmentCount = 30;
		res = 6;
	}
	else
	{
		g_SegmentCount = 10;
		res = 20;
	}
	/* tell OpenGL to start drawing a strip of lines */
	for (i = 0; i < g_SegmentCount; i++)
	{
		x[i] = 	Ghost_Bernstein1((float)i/g_SegmentCount) * g_ControlPointsGhost[0].m_X +
			Ghost_Bernstein2((float)i/g_SegmentCount) * g_ControlPointsGhost[1].m_X +
			Ghost_Bernstein3((float)i/g_SegmentCount) * g_ControlPointsGhost[2].m_X +
			Ghost_Bernstein4((float)i/g_SegmentCount) * g_ControlPointsGhost[3].m_X +
			Ghost_Bernstein5((float)i/g_SegmentCount) * g_ControlPointsGhost[4].m_X;
		y[i] = 	Ghost_Bernstein1((float)i/g_SegmentCount) * g_ControlPointsGhost[0].m_Y +
			Ghost_Bernstein2((float)i/g_SegmentCount) * g_ControlPointsGhost[1].m_Y +
			Ghost_Bernstein3((float)i/g_SegmentCount) * g_ControlPointsGhost[2].m_Y +
			Ghost_Bernstein4((float)i/g_SegmentCount) * g_ControlPointsGhost[3].m_Y +
			Ghost_Bernstein5((float)i/g_SegmentCount) * g_ControlPointsGhost[4].m_Y;

	}
	/* set an appropriate colour for the line */
	glBegin(GL_TRIANGLE_STRIP);
	for (degree=0;degree<360;degree+=res)
	{

		for (i = 0; i <g_SegmentCount; i++)
		{
			/* the interpolated points on the line */
			if(i==0)
			{
				glNormal3f(0.0f,1.0f,0.0f);
			}
			else
			{
				gradient= (x[i-1]-x[i])/(y[i]-y[i-1]);
				mag = sqrt(1+pow(gradient,2));
				glNormal3f(cos((degree)/180.0*Pi)/mag,gradient/mag,sin((degree)/180.0*Pi)/mag);
			}
			if((degree<=10 || degree>=350) && (i>(float)g_SegmentCount/2.0-2 && i<(float)g_SegmentCount/2.0+2))
			{
				glColor3f(1.0,1.0f,1.0f);
			}
			else
			{
				//blue if they are being chased
				if(hunt_flag == TRUE)
					glColor3f(0.0f,0.0f,1.0f);
				else
					glColor3f((float)color[0],(float)color[1],(float)color[2]);
					
			}
			glVertex3f(x[i]*cos((degree)/180.0*Pi),y[i],x[i]*sin((degree)/180.0*Pi));
			if(i==0)
			{
				glNormal3f(0.0f,1.0f,0.0f);
			}
			else
			{
				gradient= (x[i-1]-x[i])/(y[i]-y[i-1]);
				mag = sqrt(1+pow(gradient,2));
				glNormal3f(cos((degree+res)/180.0*Pi)/mag,gradient/mag,sin((degree+res)/180.0*Pi)/mag);
			}
			if((degree+res<=10 || degree+res>=350) && (i>(float)g_SegmentCount/2.0-2 && i<(float)g_SegmentCount/2.0+2))
			{
				glColor3f(1.0,1.0f,1.0f);
			}
			else
			{
				//blue if they are being chased
				if(hunt_flag == TRUE)
					glColor3f(0.0f,0.0f,1.0f);
				else
					glColor3f((float)color[0],(float)color[1],(float)color[2]);
			}
			glVertex3f(x[i]*cos((degree+res)/180.0*Pi),y[i],x[i]*sin((degree+res)/180.0*Pi));
		}
	
	/* finish drawing the lines */
	}
	glEnd();
 }
