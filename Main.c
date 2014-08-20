/*
 * 3D PacMan Game
 * University of Melbourne
 * Semester 2 - 2011
 */

/* Ordinary C stuff */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include <string.h>

#include <time.h>

//#include <windows.h>

/* GLUT headers */
//#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
//OSX import
#ifdef OSX
    #include <GLUT/glut.h>
#else
    #include "/usr/include/GL/glut.h"
#endif

//Maths library - remember to use -lm if building with GCC
#include <math.h>

//Our very own resources
#include "smalloc.h"
#include "includes.h"
#include "readfile.h"

#include "timer.h"
#include "camera.h"

#include "DrawPacman.h"
#include "DrawGhost.h"


/************ GLOBALS AND DEFINES ***************/


//______________________________________________
//GAME FLAGS AND OPTIONS
int pause_flag = TRUE; //is the game paused
int game_active = FALSE; //has the game started
int classic_mode = TRUE; //old school mode
int auto_move = FALSE; //legacy movement flag
int show_fps = TRUE;
int game_anim_flag = FALSE;
int shaky_cam = FALSE; //camera centered on player
int g_UseWireframe = FALSE;
//______________________________________________

int ediblex, ediblez; //stores the (x,z) location of the current edible object

//Added a structure to handle the camera variables.
Camera camera[1];
Player players[5];

int pli; //player iterator

//angle change 0 is for left and right (turning),
//angle change 1 is for up and down (climbing)
float angle_change[] = {0,0};
float change = 0;
int camera_speed = 3;
float change_height = 0;
int hunt_flag = FALSE;



//lighting variables

GLfloat LightAmbient[]=  { 0.5f, 0.5f, 0.5f, 1.0f };	// Ambient Light Values
GLfloat LightDiffuse[]=	 { 1.0f, 1.0f, 1.0f, 1.0f };	// Diffuse Light Values
GLfloat LightPosition[]= { 0.0f, 1.0f, 1.0f, 0.0f };
/************ FUNCTION PROTOTYPES ***************/

/*
 GLUT callbacks. These will be called by GLUT when something
 important happens. In our InitialiseGLUT function, we tell
 GLUT which function to call when which event happens.
 */
void OnWindowResize(int newWidth, int newHeight);
void OnKeyboardAction(unsigned char key, int mousex, int mousey);
void OnMouseAction(int button, int state, int mousex, int mousey);
void OnMouseDrag(int mousex, int mousey);
void OnDrawScene(void);

/* Our initialistation routines */
void InitialiseGLUT(int argc, char **argv);
void InitialiseScene(void);
void InitialiseOpenGL(void);

/* Our per-frame updating and rendering */
void UpdateFrame(void);
GLfloat getHeight(int, int);
void drawLandscape();
void setupSurfaceVertex(int, int, int);
void setNormal(int, int, int);
int updateHeading(int, int);
void updateCamera();
void updatePlayer();
Player initPlayer(int,int,int, float[3],int);

int reverseHeading(int);
int is_stopped(int);
int getNextXNode(int,int);
int getNextZNode(int,int);
float get_angle_change(int, int);

void create_adjacency_node(Aj_list *, Aj_list *, int, int);
Aj_list * create_adjacency_list(void);
Aj_list * getNextNode(int, Aj_list *);
int get_edible_pos();

float get_angle_between_nodes(float , float );
void create_default_pill_map();
float get_distance_from_camera(int,float,int);
double get_distance_from_player(int, int);
int get_chase_heading(int, double);

//aj list.
Aj_list *ajlist;



//this is used as part of the itoa function
//some compilers don't include it.
 void reverse(char s[])
 {
     int i, j;
     char c;

     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

//sometimes itoa isn't built in to the compiler

void itoa(int n, char* s, int radix)
 {
     int i, sign;

     if ((sign = n) < 0)  //record sign 
         n = -n;          //make n positive 
     i = 0;
     do {       //generate digits in reverse order 
         s[i++] = n % 10 + '0';   //get next digit 
     } while ((n /= 10) > 0);     //delete it 
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }



//struct for items (pills etc)
struct node{
	int x_pos;
	int z_pos;
	int points;
	struct node *next;
};

struct node *pills = NULL;


Aj_list * create_adjacency_list()
{
	int i,j;
	Aj_list *start;
	Aj_list *tmp;
	start = smalloc(sizeof(Aj_list) * 2 * (DIMENS*DIMENS));
	
	tmp = start;

	for(i=0;i<DIMENS;i++)
	{
		for(j=0;j<DIMENS;j++)
		{
			tmp->x = j;
			tmp->z = i;
			//printf("X: %d \t Z: %d\n",tmp->x, tmp->z);

			//check if it's a turning node
			if(i % 8 == 0 && j % 8 == 0)
			{
				tmp->turning_node = TRUE;
			}
			else
			{
				tmp->turning_node = FALSE;
			}

			if(i == 0)
			{
				//tmp->north = NULL;
				tmp->north = tmp;
			}
			else
			{
				tmp->north = tmp - DIMENS;
			}

			if(i == DIMENS - 1)
			{
				//tmp->south = NULL;
				tmp->south = tmp;
			}
			else
			{
				tmp->south = tmp + DIMENS;
			}

			if(j == 0)
			{
				//tmp->west = NULL;
				tmp->west = tmp;
			}
			else
			{
				tmp->west = tmp - 1;
			}

			if(j == DIMENS - 1)
			{
				//tmp->east = NULL;
				tmp->east = tmp;
			}
			else
			{
				tmp->east = tmp + 1;
			}



			tmp->next = tmp +1;
			//next node in memory
			tmp += 1;

		}
	}

	//set the next pointer of the last node to null.
	(tmp-1)->next = NULL;

	return (Aj_list *)start;
}

void print_aj_list(Aj_list *start)
{
	int i = 0;
	Aj_list *tmp;
	tmp = start;
	while (tmp != NULL)
	{

		if(tmp->turning_node)
			printf("+");
		else
			printf("_");
		//printf("%d\t%d",tmp->x,tmp->z);

		/*
		if(tmp->west != NULL)
			printf("west: %d \t %d\n",tmp->west->x,tmp->west->z);
		else
			printf("west: NULL\n");
		*/
		if(i == 128 )
		{
			printf("\n");
			i=0;
		}
		else
			i++;



		tmp=tmp->next;
	}
}


//add a pill to the list
struct node* add(struct node *head, int x,int z, int points){
	struct node *tmp;

	if(head == NULL)
	{
		head=(struct node *)smalloc(sizeof(struct node));
		head-> x_pos = x;
		head-> z_pos = z;
		head-> points = points;
		head-> next = NULL;
	}
	else
	{
		tmp = head;
		
		while (tmp->next != NULL)
		{
			tmp = tmp-> next;
		}
		tmp->next = (struct node *)smalloc(sizeof(struct node));
		tmp->next-> x_pos = x;
		tmp->next-> z_pos = z;
		tmp->next-> points = points;
		tmp->next-> next = NULL;
	}
	return head;
}


void printlist(struct node *head)
{
	struct node *current;
	current = head;
	if(current!= NULL)
	{
		do
		{
			printf("x:%d\t",current->x_pos);
			printf("z:%d\t",current->z_pos);
			printf("points:%d\t\n\n",current->points);
			current = current->next;
		} while (current!= NULL);
		printf("\n");
	}
	else
		printf("The list is empty\n");

}

void destroy(struct node *head) 
{
	struct node *current, *tmp;
	
	current = head->next;
	head->next = NULL;
	while(current != NULL) {
		tmp = current->next;
		free(current);
		current = tmp;
	}
}

struct node * remove_pill(struct node *head, struct node *pill)
{
	struct node *current;
	struct node *tmp;
	current = head;

	//check if we are removing the head
	if(head==pill)
	{
		current=pill->next;
		free(pill);
		return current;
	}
	else
	{
		while(current->next != NULL)
		{
			if(current->next==pill)
			{
				//remove current->next
				tmp = current->next;
				current->next = current->next->next;
				free(tmp);
				return head;
			}
			current = current->next;
		}
	}
	return head;
}


int get_random_heading()
{
	return rand() % 4;
}

int get_move_countdown()
{
	return rand() % 150;
}



///////////////////////////////////////////
Player initPlayer(int nodex,int nodez,int heading, float color[3], int is_ghost)
	
{
	Player player;
	player.counter = 0;
	player.heading=heading;
	player.nextheading=heading;
	player.score=0;

	//set the current node pointer
	player.current_node = ajlist + (abs(nodex) * DIMENS) + abs(nodez);
	player.next_node = getNextNode(heading, player.current_node);

	//player.heightdiff=getHeight(player.next_node_x,player.next_node_z)-getHeight(nodex,nodez);

	player.x_pos =  -player.current_node->x;
	player.z_pos =  -player.current_node->z;

	player.color[0] = color[0];
	player.color[1] = color[1];
	player.color[2] = color[2];

	player.is_ghost = is_ghost;

	player.stopped = FALSE;
	player.breaking = FALSE;

	player.move_count_down = 0;

	player.slow_down_factor = 1;

	player.lives = 3;

	player.y_pos=getHeight(nodex,nodez);



	return player;
}

void player_reposition(int p, int nodex, int nodez)
{
	players[p].current_node = ajlist + (abs(nodex) * DIMENS) + abs(nodez);
	players[p].next_node = getNextNode(players[p].heading, players[p].current_node);
	players[p].nextheading = players[p].heading;
	players[p].x_pos =  -players[p].current_node->x;
	players[p].z_pos =  -players[p].current_node->z;
	players[p].counter = 0;
}

Aj_list * getNextNode(int heading, Aj_list *current_node)
{
	Aj_list *result = NULL;
	switch( heading )
	{
		case 0:
			result = current_node->north;
			break;
		case 1:
			result = current_node->east;
			break;
		case 2:
			result = current_node->south;
			break;
		case 3:
			result = current_node->west;
			break;
		default :
			result = NULL;
			break;
	}
	if(result == NULL)
	{
		return current_node;
	}
	else
	{
		return result;
	}
}



int is_stopped(int player_num)
{
	//check if not moving
	if(players[player_num].current_node == players[player_num].next_node)
	{
		return TRUE;
	}
		return FALSE;
}

void updatePlayer()
{
	//printf("X: %2.2f\n",players[0].x_pos);
	//printf("Z: %2.2f\n",players[0].z_pos);

	int p;
		//allowing the player to stop
		if(players[0].breaking == TRUE && players[0].next_node->turning_node == TRUE)
		{
			players[0].stopped = TRUE;
			players[0].breaking = FALSE;
		}
	
	//check for collisions here.
	for (p=0;p<5;p++) {

		if(pause_flag == FALSE)
		{

			//set the height diff to see if a player is on a hill
			players[p].heightdiff=getHeight(players[p].next_node->x,players[p].next_node->z)-getHeight(players[p].current_node->x,players[p].current_node->z);

			if(players[p].counter == 0)
			{
				players[p].counter = players[p].slow_down_factor;
			}
			else if( players[p].next_node != players[p].current_node)
			{
				players[p].y_pos = -getHeight(players[p].current_node->x,players[p].current_node->z) - players[p].heightdiff * ((float)(0*players[p].slow_down_factor + players[p].counter)/(float)players[p].slow_down_factor);
				players[p].counter--;

				if (players[p].heading == 0) {
					players[p].z_pos = (float)(-(players[p].current_node->z) - (float)(0*players[p].slow_down_factor + players[p].counter)/(float)(players[p].slow_down_factor+1));
				}
				else if (players[p].heading == 1) {
					players[p].x_pos = (float)(-(players[p].current_node->x) + (float)(0*players[p].slow_down_factor + players[p].counter)/(float)(players[p].slow_down_factor+1));
				}
				else if (players[p].heading == 2) {
					players[p].z_pos = (float)(-(players[p].current_node->z) + (float)(0*players[p].slow_down_factor + players[p].counter)/(float)(players[p].slow_down_factor+1));
				}
				else if (players[p].heading == 3) {
					players[p].x_pos = (float)(-(players[p].current_node->x) - (float)(0*players[p].slow_down_factor + players[p].counter)/(float)(players[p].slow_down_factor+1));
				}

				continue;
			}


			if (players[p].heading == 0) {
				players[p].z_pos = (float)(-(players[p].next_node->z) );
			}
			else if (players[p].heading == 1) {
				players[p].x_pos = (float)(-(players[p].next_node->x) );
			}
			else if (players[p].heading == 2) {
				players[p].z_pos = (float)(-(players[p].next_node->z) );
			}
			else if (players[p].heading == 3) {
				players[p].x_pos = (float)(-(players[p].next_node->x) );
			}


			//set height for each player.
			players[p].y_pos = -getHeight(players[p].next_node->x,players[p].next_node->z);

			//check if we are the player
			//so ghosts dont eat our goodies
			if(players[p].is_ghost == FALSE)
			{
				struct node *tmp_pills;
				int num_ghosts;
				//check if we are hitting a ghost.
				for(num_ghosts=1;num_ghosts<5;num_ghosts++)
				{
					if((p != num_ghosts) &&
					(abs((players[p].current_node->x) - (players[num_ghosts].current_node->x)) < 3) &&
					(abs((players[p].current_node->z) - (players[num_ghosts].current_node->z)) < 3)
					)
					{
						if(hunt_flag == TRUE)
						{
							int max_dimens = -(DIMENS -1);
							//get some points
							players[p].score += 50;
							//move the ghost to a random corner
							player_reposition(num_ghosts,(rand() % 2) * max_dimens,(rand() % 2) * max_dimens);
						}
						else
						{
							//when you die
							players[0].lives--;
							player_reposition(0,-(DIMENS -1)/2,-(DIMENS -1)/2);
							//players[0].stopped = TRUE;

							//reposition the ghosts
							player_reposition(1,0,-(DIMENS -1));
							player_reposition(2,-(DIMENS -1),-(DIMENS -1));
							player_reposition(3,-(DIMENS -1),0);
							player_reposition(4,0,0);
						}
					}
				}
				//detects collision with edible item and updates score
				if (	((abs(players[p].next_node->x - ediblex)) < 3)
						&& ((abs(players[p].next_node->z - ediblez)) < 3) )
				{

						players[p].score+=10;
						//printf("Player %d's Score: %d\n", (p+1), players[p].score);
						ediblex = get_edible_pos();
						ediblez = get_edible_pos();
				}

				//check to see if we are colliding with the pills
				tmp_pills = pills;
				while(tmp_pills != NULL)
				{
					if (	((abs(players[p].next_node->x - tmp_pills->x_pos)) < 3)
							&& ((abs(players[p].next_node->z - tmp_pills->z_pos)) < 3) )
					{
						players[p].score+=tmp_pills->points;
						//check if it was a power pill
						if(tmp_pills->points == 5)
						{
							int tmp_ghost = 1;
							hunt_flag = TRUE;
							for(tmp_ghost = 1;tmp_ghost<5;tmp_ghost++)
							{
								players[tmp_ghost].move_count_down = 600;
							}
						}
						//printf("Player %d's Score: %d\n", (p+1), players[p].score);
						pills = remove_pill(pills, tmp_pills);
						break;
					}
					//next
					tmp_pills = tmp_pills->next;
				}
				//check if we have finished the game
				if(pills == NULL)
				{
					create_default_pill_map();
					players[0].lives++;
					players[0].score+=100;
				}
			}
			//this is ghost only stuff!!!!
			else
			{
				//check distance from player
				double distance = get_distance_from_player(players[p].current_node->x,players[p].current_node->z);
				//Slow down ghosts if they are climbing a hill!
				float angle = get_angle_between_nodes(getHeight(players[p].current_node->x,players[p].current_node->z),getHeight(players[p].next_node->x,players[p].next_node->z));

				//check if they are being hunted
				if(hunt_flag == TRUE && players[p].move_count_down <= 0)
				{
					hunt_flag = FALSE;
				}

				if(angle > 0)
				{
					players[p].slow_down_factor = 8;
				}
				else
				{
					players[p].slow_down_factor = 1;
				}

				//check if they are close to the player
				if(distance < 50)
				{

				//chase the player or run away
					if(hunt_flag == TRUE)
					{
						players[p].heading = reverseHeading(get_chase_heading(p, distance));
					}
					else
					{
						players[p].heading = get_chase_heading(p, distance);
					}

					players[p].move_count_down--;
				}
				else
				{
				//random movement
					//move the ghost if they are ready
					if(players[p].move_count_down <= 0)
					{
						players[p].heading = get_random_heading();

						players[p].move_count_down = get_move_countdown();
					}
					else
					{
						players[p].move_count_down--;
					}
				}



			}

				//only allow player to move within our grid
				if(players[p].is_ghost == FALSE && players[p].heading != players[p].nextheading && players[p].next_node->turning_node == TRUE)
				{
					angle_change[0] += get_angle_change(players[p].heading, players[p].nextheading);
					if(angle_change[0] > 90)
					{
						camera_speed = 7;
					}
					else
					{
						camera_speed = 3;
					}
					players[p].heading = players[p].nextheading;
				}

				//move the player
				players[p].current_node = players[p].next_node;


				if(players[p].stopped == FALSE)
				{
					players[p].next_node = getNextNode(players[p].heading, players[p].current_node);
				}
				else
				{
					players[p].counter = 0;
					players[p].next_node = 	players[p].current_node;
				}

				//printf("%d\n",players[0].nextheading);
		}
		//printf("%2.2f\n",angle_change[0]);
		//players[p].current_node = ajlist + (players[p].current_node->x * DIMENS) + players[p].current_node->z;
	}

	//printf("Z: %d \t %d\n",players[0].current_node->z, players[0].next_node->z);
	//printf("X: %d \t %d\n",players[0].current_node->x, players[0].next_node->x);

	//printf("Z: %d \t %2.2f\n",players[0].current_node->z, players[0].z_pos);
	//printf("X: %d \t %2.2f\n",players[0].current_node->x, players[0].x_pos);

	//printf("%d\n",players[0].stopped);
}

int get_chase_heading(int p, double dist)
{
	double best_dist = dist;
	double tmp_dist = dist;
	int tmp_heading = 0;

	tmp_dist = get_distance_from_player(players[p].current_node->north->x, players[p].current_node->north->z);
	printf("");
	if( tmp_dist < best_dist)
	{
		best_dist = tmp_dist;
		tmp_heading = 0;
	}

	tmp_dist = get_distance_from_player(players[p].current_node->east->x, players[p].current_node->east->z);
	if( tmp_dist < best_dist)
	{
		best_dist = tmp_dist;
		tmp_heading = 1;
	}

	tmp_dist = get_distance_from_player(players[p].current_node->south->x,players[p].current_node->south->z);
	if( tmp_dist < best_dist)
	{
		best_dist = tmp_dist;
		tmp_heading = 2;
	}

	tmp_dist = get_distance_from_player(players[p].current_node->west->x, players[p].current_node->west->z);
	if( tmp_dist < best_dist)
	{
		best_dist = tmp_dist;
		tmp_heading = 3;
	}

	return tmp_heading;
}

float get_angle_change(int head, int new_head)
{
	//get the new angle difference
	if(head == 0)
		{
			switch( new_head )
			{
			case 1:
				return 90.0;
				break;
			case 2:
				return 180.0;
				break;
			case 3:
				return -90.0;
				break;
			case 0:
				return 0.0;
				break;
			default:
				return 0;
				break;
			}
		}
	else if(head == 1)
	{
		switch( new_head )
		{
		case 1:
			return 0.0;
			break;
		case 2:
			return 90.0;
			break;
		case 3:
			return 180.0;
			break;
		case 0:
			return -90.0;
			break;
		default:
			return 0;
			break;
		}
	}
	else if(head == 2)
		{
			switch( new_head )
			{
			case 1:
				return -90.0;
				break;
			case 2:
				return 0.0;
				break;
			case 3:
				return 90.0;
				break;
			case 0:
				return 180.0;
				break;
			default:
				return 0;
				break;
			}
		}
	else if(head == 3)
		{
			switch( new_head )
			{
			case 1:
				return 180.0;
				break;
			case 2:
				return -90.0;
				break;
			case 3:
				return 0.0;
				break;
			case 0:
				return 90.0;
				break;
			default:
				return 0;
				break;
			}
		}
	return 0;
}

float get_angle_between_nodes(float height1, float height2)
{
	//printf("%2.2f\n",atan(height2-height1)*180/Pi);
	return atan(height2-height1)*180/Pi;
}

void updateCamera()
{
	//incase we had more cameras/players
	int c = 0;
	int cam_distance = 50;
	float angle = get_angle_between_nodes(getHeight(players[c].current_node->x,players[c].current_node->z),getHeight(players[c].next_node->x,players[c].next_node->z));
	int sign = 1;
	int angle_diff = abs(camera[c].x_angle) - abs(angle_change[1]);

	int height_diff = abs(camera[c].y_pos) - abs(change_height);

	//check for clipping so we can't see under the terrain
	if(TRUE)
	{
		change_height = getHeight(players[c].current_node->x,players[c].current_node->z) + 30;
		//printf("%2.2f");
		/*
		if (abs((players[0].y_pos/maxHeight))*100 < 30)
		{
			change_height = maxHeight * 0.50;
		}
		else if (abs((players[0].y_pos/maxHeight))*100 < 60)
		{
			change_height = maxHeight * 0.75;
		}
		else
		{
			change_height = maxHeight * 0.90;
		}
		*/
	}


	if(game_anim_flag == TRUE && height_diff <= 3 && height_diff >= -3 )
	{
		game_active = TRUE;
		pause_flag = FALSE;
		game_anim_flag = FALSE;
	}

	//camera[c].y_pos = players[c].y_pos-60;
	if(classic_mode == FALSE)
	{
		if(shaky_cam == TRUE)
		{
			camera[c].y_pos = players[c].y_pos-20;
		}
		else
		{
			if( height_diff > 3 || height_diff < -3)
			{
				if(game_anim_flag == TRUE)
					camera[c].y_pos += 1.5 * abs(height_diff)/height_diff;
				else
					camera[c].y_pos += 0.5 * abs(height_diff)/height_diff;
			}
		}


		if (angle < 0)
		{
			sign = -1;
		}

		//angle_change[1] = 20 - sign * 20 * (getHeight(players[c].current_node->x,players[c].current_node->z)/maxHeight);
		//angle_change[1] = 0;
		if(angle < -15)
		{
			angle_change[1] = -30;
		}
		else if (angle > 15)
		{
			angle_change[1] = sign*5;
		}
		else
		{
			angle_change[1] = 30;
		}

		//printf("want: %2.2f\t have: %2.2f\n",angle_change[1], camera[c].x_angle);

		if( angle_diff > 3 || angle_diff < -3)
		{
			if(game_anim_flag == TRUE)
				camera[c].x_angle -= 0.80 * abs(angle_diff)/angle_diff;
			else
				camera[c].x_angle -= 0.20 * abs(angle_diff)/angle_diff;
		}
		//camera[c].x_angle = 25;
	}
	//classic mode
	else
	{
		camera[c].y_pos = -250;
		camera[c].x_angle = 90;
		camera[c].x_pos = players[c].x_pos;
		camera[c].z_pos = players[c].z_pos;
	}

	//camera[c].y_pos = -maxHeight*5;

	//printf("%2.2f\n",angle_change[0]);
	//printf("%2.2f\n",g_AnimTimer);


	switch( players[c].heading ) 
	{
		case 0:
			if(angle_change[0] > 3 || angle_change[0] < -3)
			{
				change = angle_change[0]/abs(angle_change[0])*camera_speed;
				camera[c].y_angle += change;
				angle_change[0] -= change;
			}
			else
			{
				camera[c].y_angle = 0;
				angle_change[0] = 0;
			}
			if(classic_mode == FALSE && players[0].counter == 0)
			{
				camera[c].x_pos = players[c].x_pos;
				camera[c].z_pos = players[c].z_pos-cam_distance;
			}
			break;

		case 1:
			if(angle_change[0] > 3 || angle_change[0] < -3)
			{
				change = angle_change[0]/abs(angle_change[0])*camera_speed;
				camera[c].y_angle += change;
				angle_change[0] -= change;
			}
			else
			{
				camera[c].y_angle = 90;
				angle_change[0] = 0;
			}
			if(classic_mode == FALSE && players[0].counter == 0)
			{
				camera[c].x_pos = players[c].x_pos+cam_distance;
				camera[c].z_pos = players[c].z_pos;
			}
			break;

		case 2:
			if(angle_change[0] > 3 || angle_change[0] < -3)
			{
				change = angle_change[0]/abs(angle_change[0])*camera_speed;
				camera[c].y_angle += change;
				angle_change[0] -= change;
			}
			else
			{
				camera[c].y_angle = 180;
				angle_change[0] = 0;
			}
			if(classic_mode == FALSE && players[0].counter == 0)
			{
				camera[c].x_pos = players[c].x_pos;
				camera[c].z_pos = players[c].z_pos+cam_distance;
			}
			break;

		case 3:
			if(angle_change[0] > 3 || angle_change[0] < -3)
			{
				change = angle_change[0]/abs(angle_change[0])*camera_speed;
				camera[c].y_angle += change;
				angle_change[0] -= change;
			}
			else
			{
				camera[c].y_angle = 270;
				angle_change[0] = 0;
			}
			if(classic_mode == FALSE && players[0].counter == 0)
			{
				camera[c].x_pos = players[c].x_pos-cam_distance;
				camera[c].z_pos = players[c].z_pos;
			}
			break;
	}

}

int reverseHeading(int currentheading)
{
	switch( currentheading )
	{
	case 0:
		return 2;
		break;
	case 1:
		return 3;
		break;
	case 2:
		return 0;
		break;
	case 3:
		return 1;
		break;
	default:
		return 0;
		break;

	}
	return 0;
}

int updateHeading(int keypress, int currentheading )
{
	//keypress is either 0 or 1, (left or right)

	switch( currentheading )
	{
		case 0:
			if ( (keypress == 0) )
			return 3;
			else return 1;
			break;
		case 1:
			if ( (keypress == 0) )
			return 0;
			else return 2;
			break;
		case 2:
			if ( (keypress == 0) )
			return 1;
			else return 3;
			break;
		case 3:
			if ( (keypress == 0) )
			return 2;
			else return 0;
			break;
	}

}

/************ Terrain Generation FUNCTIONS ***************/

void initLandscape(GLfloat A[DIMENS][DIMENS])
{
	int x, y;
	for (y = 0; y < DIMENS; y++) {
		for (x = 0; x < DIMENS; x++) {
			A[x][y] = 0.0;
		}
	}
}

double multRnd(double delta)
{
	static double alpha = 0.5;
	static double c = 0.6;//Hills
	return c * ((pow(delta, -(log(alpha)) / log(2))));
}

void raiseTerrain(GLfloat A[DIMENS][DIMENS], int s)
{
	int i, x, y;
	for (i = 1; i <= s; i++) 
	{
		int p = (DIMENS - 1) / (1 << (i - 1));
		for (y = 0; y < DIMENS; y += p) {
			for (x = 0; x < DIMENS; x += p) {
				if (x < DIMENS - p) {
					float rnd = (float)(rand()%100) / 100 - 0.5;
					A[x + p/2][y] = (A[x][y]+A[x + p][y]) / 2+multRnd(p)*rnd;
				}
				if (y < DIMENS - p) {
					float rnd = (float)(rand()%100) / 100 - 0.5;
					A[x][y + p/2] = (A[x][y]+A[x][y + p]) / 2+multRnd(p)*rnd;
				}
			}
		}
		for (y = 0; y < DIMENS - p; y += p) {
			for (x = 0; x < DIMENS - p; x += p) {
				float rnd = (float)(rand()%100) / 100 - 0.5;
				A[x + p/2][y + p/2] = 
					(A[x + p/2][y] +
					A[x][y + p/2] +
					A[x + p][y + p/2] +
					A[x + p/2][y + p]) / 4 + 
					multRnd(p) * rnd;
			}
		}
	}
	minHeight = A[0][0];
	maxHeight = A[0][0];
	for (y = 0; y < DIMENS; y++) {
		for (x = 0; x < DIMENS; x++) {
			if (A[x][y] < minHeight) minHeight = A[x][y];
			if (A[x][y] > maxHeight) maxHeight = A[x][y];
		}
	}
	
}


void colourLandscape(GLfloat height)
{
	static GLfloat low[3] = {0.15, 0.65, 0.15};
	static GLfloat mid[3] = {0.4, 0.35, 0.22};
	static GLfloat high[3] = {1.0, 1.0, 1.0};
	GLfloat highi = maxHeight;
	GLfloat lowi = minHeight;
	GLfloat midi = (2*maxHeight + minHeight) / 3.0;
	GLfloat colour[3];

	if (height > midi) {
		GLfloat factor = (height - midi) / (highi - midi);
		colour[0] = (1 - factor) * mid[0] + factor * high[0];
		colour[1] = (1 - factor) * mid[1] + factor * high[1];
		colour[2] = (1 - factor) * mid[2] + factor * high[2];
	}
	else {
		GLfloat factor = (height - lowi) / (midi - lowi);
		colour[0] = (1 - factor) * low[0] + factor * mid[0];
		colour[1] = (1 - factor) * low[1] + factor * mid[1];
		colour[2] = (1 - factor) * low[2] + factor * mid[2];
	}

	glColor3f(colour[0], colour[1], colour[2]);
}



void setNormal(int x, int y, int p)
{
	GLfloat dx, dy;

	if (x >= p && x < DIMENS - p - 1)
		dx = surface[x-p][y] - surface[x+p][y];
	else if (x < DIMENS - p - 1)
		dx = 2 * (surface[x][y] - surface[x+p][y]);
	else if (x >= p)
		dx = 2 * (surface[x-p][y] - surface[x][y]);

	if (y >= p && y < DIMENS - p - 1)
		dy = surface[x][y-p] - surface[x][y+p];
	else if (y < DIMENS - p - 1)
		dy = 2 * (surface[x][y] - surface[x][y+p]);
	else if (y >= p)
		dy = 2 * (surface[x][y-p] - surface[x][y]);

	glNormal3f(dx, p, dy);
}

GLfloat getHeight(int x, int z)
{

	if ( (x<DIMENS) && (-1<x) && (z<DIMENS) && (-1<z)) {
		return surface[x][z];}
	else return surface[0][0];
	
}


void setupSurfaceVertex(int x, int y, int p)
{
	setNormal(x, y, p);
	colourLandscape(surface[x][y]);
	glVertex3f(x, getHeight(x, y), y);
}

void drawLandscape()
{
	int x, y;
	
	for (y = 0; y < (DIMENS - 1); y++) 
	{
		glBegin(GL_TRIANGLE_STRIP);
		setupSurfaceVertex(0, y, 1);

		for (x = 0; x < (DIMENS - 1); x++) 
		{
			setupSurfaceVertex(x, y + 1, 1);
			setupSurfaceVertex(x + 1, y, 1);
		}
		setupSurfaceVertex(DIMENS - 1, y + 1, 1);
		glEnd();
	}
}



/************ CALLBACK FUNCTIONS ***************/

/* Called whenever the size of the window changes */
void OnWindowResize(int newWidth, int newHeight)
{
	/* save the window width and height for later use */
	g_WindowWidth = (float)newWidth;
	g_WindowHeight = (float)newHeight;

	/*
	 set up the OpenGL projection matrix, including updated aspect ratio
	 note: "aspect ratio" means "width / height"
	 note: if you wanted your image to "stretch" to fill the window, you would
	       not need to adjust the aspect ratio here.
	 */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FieldOfViewInDegrees, g_WindowWidth / g_WindowHeight,
				   NearZPlane, FarZPlane);
}





void print_angles(int camno)
{
	printf("AngleCam%d: x:%f, y:%f\n", camno, camera[camno].x_angle, camera[camno].y_angle);
	printf("PosCam%d: x:%f, y:%f, z:%f\n", camno, camera[camno].x_pos, camera[camno].y_pos, camera[camno].z_pos);
	printf("PlyrPos%d: x:%f, y:%f, z:%f\n", camno, players[camno].x_pos, players[camno].y_pos, players[camno].z_pos);
	printf("Plyr0: heightdiff:%f\n", players[0].heightdiff);
}

/* Called whenever a keyboard button is pressed */
void OnKeyboardAction(unsigned char key, int mousex, int mousey)
{
	/* handle input related to display lists */
	unsigned char keytest = tolower(key);

	//start the game (enter key)
	if (game_active == FALSE && key == 13)
		{
			game_anim_flag = TRUE;
			classic_mode = FALSE;
		}

	/* refresh landscape */
	else if (key == 'r')
	{
		initLandscape(surface);
		raiseTerrain(surface, ITR);
		//players[0].y_pos = -getHeight(players[0].node_x,players[0].node_z);
		players[0].y_pos = -getHeight(players[0].current_node->x,players[0].current_node->z);
	}
	/* allow the user to quit with the keyboard */
	else if (keytest == 'q')
	{
		exit(0);
	}
	else if (keytest == 'w')
	{
		g_UseWireframe ^= 1;
	}
	//pause the game
	else if (keytest == 'p')
	{
		pause_flag ^= 1;
	}
	else if (keytest == 's')
	{
		players[0].stopped ^= 1;
	}
	else if (keytest == 'f')
	{
		show_fps ^= 1;
	}
	else if (keytest == 'c')
	{
		classic_mode ^= 1;
	}
	else if (keytest == 'v')
	{
		shaky_cam ^= 1;
	}
	else if (keytest == 'm')
	{
		initLandscape(surface);
		players[0].y_pos = -getHeight(players[0].current_node->x,players[0].current_node->z);
	}

}

/*Handle Arrow keys*/
void OnSpecialAction(int key, int mousex, int mousey)
{
	float g_deg2Rad = 0.017453292519943f;
	float speed=0.5;

	//Turn left
	if (key == GLUT_KEY_LEFT)
	{
		players[0].nextheading = updateHeading(0,players[0].heading);
	}

	//Turn right
	else if (key == GLUT_KEY_RIGHT)
	{
		players[0].nextheading = updateHeading(1,players[0].heading);
	}

	//Turn right
	else if (key == GLUT_KEY_UP)
	{
		players[0].stopped = FALSE;

		//players[0].next_node_x = getNextXNode(players[0].nextheading, players[0].node_x);
		//players[0].next_node_z = getNextZNode(players[0].nextheading, players[0].node_z);
		players[0].next_node = getNextNode(players[0].heading, players[0].current_node);
	}

	//Reverse
	else if (key == GLUT_KEY_DOWN)
	{
		if(auto_move == TRUE)
		{
			players[0].nextheading = reverseHeading(players[0].heading);
		}
		else
		{
			if(players[0].stopped == FALSE)
			{
				//players[0].stopped = TRUE;
				players[0].breaking = TRUE;
			}
			else
			{
				players[0].nextheading = reverseHeading(players[0].heading);
				//rotate camera by 180 degrees here, anim not working for this turn.
				//players[0].heading = reverseHeading(players[0].heading);

				//players[0].next_node_x = getNextXNode(players[0].nextheading, players[0].node_x);
				//players[0].next_node_z = getNextZNode(players[0].nextheading, players[0].node_z);
				players[0].next_node = getNextNode(players[0].nextheading, players[0].current_node);

				players[0].stopped = FALSE;
			}
		}
	}
}

/* Called whenever a mouse button is pressed or released */
void OnMouseAction(int button, int state, int mousex, int mousey)
{
	/*
	 button is one of GLUT_LEFT_BUTTON, GLUT_RIGHT_BUTTON or GLUT_MIDDLE_BUTTON
	 state is one of GLUT_UP and GLUT_DOWN
	 */
}

/* Called when the mouse is moved while a button is held down */
void OnMouseDrag(int mousex, int mousey)
{
	/*
	 you will also need to monitor OnMouseAction to see what
	 buttons are being pressed
	 */
}

/************ INITIALISATION ***************/

/* Start GLUT, open a window to draw into, etc */
void InitialiseGLUT(int argc, char **argv)
{
	/* initialize glut */
	glutInit(&argc, argv);

	/* specify the display for our OpenGL application */
	glutInitDisplayMode(GLUT_RGBA				/* Use the RGBA colour model */
						| GLUT_DOUBLE			/* Use double buffering */
						| GLUT_DEPTH);			/* Use a depth buffer */

	/* define the size of the window */
	glutInitWindowSize(g_WindowWidth, g_WindowHeight);

	/* the position where the window will appear */
	glutInitWindowPosition(300,300);


	/* create the window, giving it a title in the process */
	glutCreateWindow(g_ApplicationName);

	/* if you want fullscreen, call this function */
	//glutFullScreen();

	/*
	 Tell GLUT what functions it should call when various things happen
	 For instance, when the window needs to be redrawn, OnDrawScene will
	 be called. When a button is pressed on the keyboard, OnKeyboardAction
	 will be called.
	 This process is known as "registering callbacks". The functions we
	 provide are called "callbacks" because GLUT "calls" them when important
	 stuff happens, thus passing control "back" to us.
	 */
	glutDisplayFunc(OnDrawScene);
	glutReshapeFunc(OnWindowResize);
	glutKeyboardFunc(OnKeyboardAction);
	glutSpecialFunc(OnSpecialAction);
	glutMouseFunc(OnMouseAction);
	glutMotionFunc(OnMouseDrag);
}

/* Do any one-time OpenGL initialisation we might require */
void InitialiseOpenGL()
{

	/* define the background, or "clear" colour, for our window  */
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	/*
	 Enable depth testing. This means that objects in the distance
	 won't get drawn on top of closer objects, even if they are sent
	 to the graphics card after the closer objects are sent. The depth
	 testing is accomplished by the use of a Z-Buffer.
	 */
	glEnable(GL_DEPTH_TEST);

	/*
	 here we could do other "one time" initialisation routines
	 for instance, we could enable lighting, or disable gouraud shading,
	 or anything else we can imagine!
	 */
	
	 glShadeModel(GL_SMOOTH);
    
     glEnable(GL_LIGHTING);              // Set up ambient light.
     glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightAmbient);
    
     glEnable(GL_LIGHT0);                // Set up sunlight.
     glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
	 glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
   
     glEnable(GL_COLOR_MATERIAL);        // Configure glColor().
     glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

}

/* Create all the objects, textures, geometry, etc, that we will use */
void InitialiseScene(void)
{
	//int i, j;
	/*Create 1 camera. The first one is one one side of the screen and the second is on the other side
	Because the second is on the opposite side, we need to rotate its view by 180 degrees*/
	camera[0]=InitCamera(0, 0, 0, 0, 0);
}

/************ PER FRAME PROCESSING AND RENDERING ***************/

//this function draws text to the screen in 3D!
void drawText(const char * message)
{
	glColor3f(1.0f, 0.0f, 0.0f);
	glScalef(0.1, 0.1, 0.1);
	while (*message) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN,*message++);
	}
}


//stolen from:
//https://users.cs.jmu.edu/bernstdh/web/common/lectures/openglexamples/text/labelsGLUT.c
void beginText()
{
   glMatrixMode(GL_PROJECTION);

   // Save the current projection matrix
   glPushMatrix();

   // Make the current matrix the identity matrix
   glLoadIdentity();

   // Set the projection (to 2D orthographic)
   gluOrtho2D(0, g_WindowWidth, 0, g_WindowHeight);

   glMatrixMode(GL_MODELVIEW);
}

void endText()
{
   glMatrixMode(GL_PROJECTION);

   // Restore the original projection matrix
   glPopMatrix();

   glMatrixMode(GL_MODELVIEW);
}

//this function draws text to the screen in 2D.
void drawString(float x, float y, char *string)
{
	char* c;
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2f(x, y);
	for (c=string; *c != '\0'; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}
	glEnable(GL_LIGHTING);
}


float get_rotation_angle(int player_num)
{
	switch( players[player_num].heading )
		{
			case 0:
				return 90.0f;
				break;
			case 1:
				return 0.0f;
				break;
			case 2:
				return 270.0f;
				break;
			case 3:
				return 180.0f;
				break;
		}
	return 0.0f;
}

float get_distance_from_camera(int x_input, float y_input, int z_input)
{
	float x,y,z;
	x = (double)x_input + (double)camera[0].x_pos;
	y = (double)y_input + (double)camera[0].y_pos;
	z = (double)z_input + (double)camera[0].z_pos;

	return sqrt(   pow(x,2.0) + pow(y,2.0) + pow(z,2.0)   );
}

double get_distance_from_player(int x_input, int z_input)
{
	double x,z;
	x = x_input - players[0].current_node->x;
	z = z_input - players[0].current_node->z;

	return sqrt(   pow(x,2.0) + pow(z,2.0)   );
}


/*
 Go through each screen, change the viewport and perspective, move the camera
 to the right spot and draw stuff. Easy
 */
void OnDrawScene(void)
{
	static float rotate=0;
	int screen=0;
	float distance = 0;
	struct node *tmp_pills;
	char mander[8];
	char meleon[32] = {' ','L','i','v','e','s',':',' '};
	char izard[32] = {'S','c','o','r','e',':',' '};
	int pill_size = 1;

	InitialisePacmanControlPoints();
	InitialiseGhostControlPoints();


	updateCamera();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

	glViewport(0, 0, g_WindowWidth, g_WindowHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (g_UseWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Also we have to change the aspect ratio to match the new window height or everything ends up really skewed
	gluPerspective(FieldOfViewInDegrees, g_WindowWidth / (g_WindowHeight), NearZPlane, FarZPlane);

	/*Each screen gets rotated by its own angles, translated to its own position*/
	glMatrixMode(GL_MODELVIEW);


	glPushMatrix();
		glRotatef(camera[screen].x_angle, 1.0, 0.0, 0.0);
		glRotatef(camera[screen].y_angle, 0.0, 1.0, 0.0);
		glTranslatef(camera[screen].x_pos, camera[screen].y_pos, camera[screen].z_pos);

		glEnable(GL_NORMALIZE);


		//Draw the pacman player
		//and the ghosts
		for (pli=0; pli<5; pli++)
		{
			glPushMatrix();
				glTranslatef(-players[pli].x_pos, -players[pli].y_pos+3,  -players[pli].z_pos);
				glColor3f(players[pli].color[0], players[pli].color[1], players[pli].color[2]);

				//rotate based on direction heading towards
				glRotatef(get_rotation_angle(pli), 0.0, 1.0, 0.0);

				if(players[pli].is_ghost == TRUE)
				{
					//***SPLINES
					glTranslatef(0.0, 0.0, -2.0);
					glRotatef(90.0, 0.0, 1.0, 0.0); 
					glRotatef(-90.0, 0.0, 1.0, 0.0);//needed this because it was 90 degrees to the side
					glScalef(10.010f,10.010f,10.010f); //also was printing way too large
					distance = get_distance_from_camera(players[pli].x_pos,((double)getHeight(players[pli].x_pos,players[pli].z_pos)),players[pli].z_pos);
					//do some shit with distance here
					DrawGhost(distance, players[pli].color,hunt_flag);
					//_______________________________
					//old rendering
					//glRotatef(-90.0, 1.0, 0.0, 0.0);
					//glutSolidCone(3, 10, 20, 20);
					//________________________________
				}
				else
				{
					//***SPLINES

					glTranslatef(0.0, 0.0, -2.0);
					glRotatef(-20.0, 0.0, 0.0, 1.0);
					glScalef(10.10f,10.10f,10.10f);

					DrawPacman();
					//_______________________________
					//old rendering
					//glutSolidSphere(3,20,20);
					//________________________________
				}
			glPopMatrix();
		}

			//draw pill on each node
			tmp_pills = pills;

			while(tmp_pills != NULL)
			{
				glPushMatrix();
					glTranslatef((tmp_pills->x_pos), (getHeight(tmp_pills->x_pos,tmp_pills->z_pos)+2), (tmp_pills->z_pos));


					//check for a power pill
					if(tmp_pills->points == 5)
					{
						pill_size = 2;
						glColor3f(1.0f, 0.5f, 1.0f);
					}
					else
					{
						pill_size = 1;
						glColor3f(1.0f, 0.0f, 1.0f);
					}

					//draw with more detail when close to the camera
					distance = get_distance_from_camera(tmp_pills->x_pos,((double)getHeight(tmp_pills->x_pos,tmp_pills->z_pos)),tmp_pills->z_pos);
					if(distance < 80)
					{
						glutSolidSphere(pill_size,20,20);
					}
					else if(distance < 140)
					{
						glutSolidSphere(pill_size,10,10);
					}
					else
					{
						glutSolidSphere(pill_size,5,5);
					}

					tmp_pills = tmp_pills->next;
				glPopMatrix();
			}


		//draw the teapot bonus edible
		glPushMatrix();
			glTranslatef(ediblex, getHeight(ediblex,ediblez)+3, ediblez);
			glColor3f(1.0f, 0.0f, 0.0f);
			distance = get_distance_from_camera(ediblex, (double)getHeight(ediblex,ediblez), ediblez);
			glutSolidTeapot(2);
		glPopMatrix();


		glPushMatrix();
			drawLandscape();
		glPopMatrix();

		//Draw the cool banners
		glPushMatrix();
			glRotatef(180.0, 0.0, 1.0, 0.0);
			glTranslatef(-100, 1+maxHeight, -140);
			drawText("PacMan 3D");
		glPopMatrix();

		glPushMatrix();
			glTranslatef(29, 1+maxHeight, -10);
			drawText("PacMan 3D");
		glPopMatrix();

		glPushMatrix();
			glRotatef(90.0, 0.0, 1.0, 0.0);
			glTranslatef(-100, 1+maxHeight, -10);
			drawText("PacMan 3D");
		glPopMatrix();

		glPushMatrix();
			glRotatef(-90.0, 0.0, 1.0, 0.0);
			glTranslatef(29, 1+maxHeight, -140);
			drawText("PacMan 3D");
		glPopMatrix();

	glPopMatrix();

	//Display the score!

	itoa(players[0].score,mander, 10);
	strcat(izard,mander);
	strcat(izard,meleon);
	itoa(players[0].lives,meleon, 10);
	strcat(izard,meleon);
	beginText();
	drawString(g_WindowWidth-220, g_WindowHeight-20, izard);
	endText();

	//pause text
	if(pause_flag == TRUE && game_active == TRUE)
	{
		beginText();
		//glScalef(2.0, 2.0, 1.0);
		drawString(g_WindowWidth/2-50, g_WindowHeight/2-50, "PAUSED");
		//glScalef(0.5, 0.5, 1.0);
		endText();
	}

	//start game text
	if(game_anim_flag == FALSE && game_active == FALSE)
	{
		beginText();
		drawString(g_WindowWidth/2-50, g_WindowHeight/2-50, "Press 'enter' to start! ");
		endText();
	}
	else if(game_active == FALSE)
	{
		beginText();
		drawString(g_WindowWidth/2-50, g_WindowHeight/2-50, "Get ready... ");
		endText();
	}


	glutSwapBuffers();
	glFlush();
	updatePlayer();
	
}

/* Our main update function - called every time we go through our main loop */
void UpdateFrame(void)
{
	/* our timing information */
	unsigned int fps;
	float dt;

	/* force another redraw, so we are always drawing as much as possible */
	glutPostRedisplay();

	/* adjust our timer, which we use for transforming our objects */
	dt = GetPreviousFrameDeltaInSeconds();
	g_AnimTimer += dt;
	if (g_AnimTimer > 1.0f)
	{
		g_AnimTimer = 0.0f;
	}

	/* timing information */
	if (ProcessTimer(&fps))
	{
		/* update our frame rate display */
		if (show_fps == TRUE)
		{
			printf("FPS: %d\n", fps);
		}
		if (pause_flag == TRUE)
		{
			//done with text on the screen now.
			//printf("PAUSED!\n");
		}
		if (players[0].stopped == TRUE)
		{
			//not really needed.
			//printf("STOPPED!\n");
		}


	}
}

int get_edible_pos()
{
	int result;
	for(result=0;(result < 8 || result > 120);result=((rand() % 9) * (DIMENS-1)/8) - 8)
	{
		//clever for loop to generate random position within our range
	}
	return result;
}

void create_default_pill_map()
{
	int pill_x,pill_z;

	//Create pill map
	for(pill_x=8;pill_x<121;pill_x+=8)
	{
		for(pill_z=8;pill_z<121;pill_z+=8)
		{
			//double check that they are only added on turning nodes
			//not really needed but used for testing.
			if((ajlist + (DIMENS * pill_x) + pill_z)->turning_node == TRUE)
			{
				//add power pills to corners
				if( (pill_z == 120 && (pill_x == 8 || pill_x == 120))  ||  (pill_z == 8 && (pill_x == 8 || pill_x == 120)))
				{
					pills = add(pills, pill_x,pill_z, 5);
				}
				//else add normal pill
				else
				{
					pills = add(pills, pill_x,pill_z, 1);
				}
			}

		}
	}
	//printlist(pills);
}


/************ GOOD OLD INT MAIN() ***************/

int main(int argc, char **argv)
{
	char line[15][15];
	int pill_x,pill_z;

	float color[3] = {0.95f, 0.9f, 0.3f};
	float g_color1[3] = {0.5f, 0.35f, 0.05f};
	float g_color2[3] = {1.0f, 0.0f, 0.0f};
	float g_color3[3] = {1.0f, 0.5f, 0.0f};
	float g_color4[3] = {0.0f, 0.5f, 1.0f};

	srand(time(NULL));
	initLandscape(surface);
	raiseTerrain(surface, ITR);

	ajlist = create_adjacency_list();
	//print_aj_list(ajlist);

	//add in the Pacman!
	//node x node z, heading
	players[0] = initPlayer(-(DIMENS -1)/2,-(DIMENS -1)/2,2,color, FALSE);
	//players[0] = initPlayer(0,0,2,color, FALSE);
	//players[0].heading = 1;

	//add in some ghosts as players...
	players[1] = initPlayer(0,-(DIMENS -1),1,g_color1, TRUE);
	players[2] = initPlayer(-(DIMENS -1),0/2,2,g_color2, TRUE);
	players[3] = initPlayer(-(DIMENS -1),-(DIMENS -1),3,g_color3, TRUE);
	players[4] = initPlayer(0,0,0,g_color4, TRUE);
	//add the fruit (teapot) to a random place
	ediblex = get_edible_pos();
	ediblez = get_edible_pos();

	//loads the map array (for items etc)
	if(argc > 1 && (read_file(argv[1], line) == TRUE))
	{
		for(pill_x=0;pill_x<15;pill_x++)
		{
			for(pill_z=0;pill_z<15;pill_z++)
			{
				//add in pills
				if((line[pill_z][pill_x]) == 'D')
				{
					pills = add(pills, pill_x*8+8,pill_z*8+8, 1);
				}
				else if((line[pill_z][pill_x]) == 'P')
				{
					pills = add(pills, pill_x*8+8,pill_z*8+8, 5);
				}
			}
		}

	}
	//if no level is given load default level
	else
	{
		create_default_pill_map();
	}

	//init splines
	InitialiseGhostControlPoints();
	InitialisePacmanControlPoints();

	InitialiseGLUT(argc, argv);
	InitialiseOpenGL();
	InitialiseTimer();
	InitialiseScene();

	glutIdleFunc(UpdateFrame);
	//players[0].y_pos = -getHeight(players[0].node_x,players[0].node_z);
	players[0].y_pos = -getHeight(players[0].current_node->x,players[0].current_node->z);
	glutMainLoop();

	/* when the window closes, we will end up here. */
	return 0;
}
