/*
 * DrawPacman.h
 *
 *  Created on: Oct 18, 2011
 *      Author: kim
 */

#ifndef DRAWPACMAN_H_
#define DRAWPACMAN_H_


#define ControlPointCount 5

static const int SelectionRadius = 3;


struct Point
{
	float m_X, m_Y;
	float m_R, m_G, m_B;
};

void InitialisePacmanControlPoints(void);

void DrawPacman(void);

#endif /* DRAWPACMAN_H_ */
