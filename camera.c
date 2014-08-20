#include <math.h>
#include "camera.h" 

float g_deg2Rad = 0.017453292519943f;

/*Set inital values*/
Camera InitCamera(float x_angle, float y_angle, float x_pos, float y_pos, float z_pos)
{
	Camera camera;
	camera.x_angle=x_angle;
	camera.y_angle=y_angle;
	camera.x_pos=x_pos;
	camera.y_pos=y_pos;
	camera.z_pos=z_pos;
	
	return camera;
}

/*Move the camera sideways*/
void CameraSide(Camera* camera, float speed)
{
	float angle_y_radians = (camera->y_angle * g_deg2Rad);
	camera->x_pos += cos(angle_y_radians) * speed;
	camera->z_pos += sin(angle_y_radians) * speed;
}

/*Rotate the camera*/
void CameraRotate(float *angle, float speed)
{
		*angle -= speed;
		if(*angle > 360)
			*angle += - 360;
		if(*angle < 0)
			*angle += 360;
}
/*Move the camera forwards/backwards*/
void CameraForward(Camera* camera, float speed)
{
		float angle_y_radians = (camera->y_angle * g_deg2Rad);
		float angle_x_radians = (camera->x_angle * g_deg2Rad); 
		camera->x_pos =	camera->x_pos - (float)sin(angle_y_radians)*speed;
		camera->z_pos = camera->z_pos + (float)cos(angle_y_radians)*speed;
		camera->y_pos = camera->y_pos + (float)sin(angle_x_radians)*speed;
}

