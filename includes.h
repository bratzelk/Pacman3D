/*
 * globals.h
 *
 *  Created on: Aug 14, 2011
 *      Author: Kim Bratzel
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

/* GLUT headers */
//#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
//OSX import
#ifdef OSX
    #include <GLUT/glut.h>
#else
    #include "/usr/include/GL/glut.h"
#endif


/************ DATA STRUCTURES ***************/
typedef struct vector_t Vector;
typedef struct aj_list_t Aj_list;

//structure of an aj list
struct aj_list_t
{
    int x;
    int z;

    struct aj_list_t *north;
    struct aj_list_t *south;
    struct aj_list_t *east;
    struct aj_list_t *west;

    int turning_node;

    //for iteration
    struct aj_list_t *next;
};

//structure of a vector
struct vector_t
{
    float x;
    float y;
    float z;
};


//structure for keeping track of the player
typedef struct {

	int lives;

	int heading;
	int nextheading;
	int score;
	float heightdiff;
	float x_pos;
	float y_pos;
	float z_pos;
	float color[3];

	int stopped;
	int breaking;

	struct aj_list_t *current_node;
	struct aj_list_t *next_node;

	int slow_down_factor;
	int counter;

	int is_ghost;
	int move_count_down;


} Player;

/************ GLOBALS AND DEFINES ***************/

#define TRUE (1==1)
#define FALSE (1==0)

#define ITR (7) //terrain itr
#define DIMENS (128+1) //terrain array dimension
GLfloat surface[DIMENS][DIMENS];
float minHeight, maxHeight;


/* the name of our application */
static const char g_ApplicationName[] = "Pacman 3D";

/* pi is a useful constant */
static const float Pi = 3.1415926535897932384626433832795f;

/*
 the width and height of our window - we will
 update these whenever the user resizes the window.
 these could be used, for instance, if you want to
 position text on the window)
 */
static float g_WindowWidth = 640.0f;
static float g_WindowHeight = 480.0f;

/* the id number of our openGL display list */
static GLuint g_DisplayList = (GLuint)(-1);

/* whether or not to render using display lists */
static unsigned char g_UseDisplayList = 0;

/* our projection settings - we use these in our projection transformation */
static const float NearZPlane = 0.1f;
static const float FarZPlane = 10000.0f;
static const float FieldOfViewInDegrees = 45.0f;

/* our rudimentary animation system */
static float g_AnimTimer = 0.0f;


#endif /* GLOBALS_H_ */
