
#ifndef CAMERA_H_
#define CAMERA_H_

/*Structure to hold camera variables*/
typedef struct 
{
	float x_angle;
	float y_angle;
	float x_pos;
	float y_pos;
	float z_pos;
}Camera;

Camera InitCamera(float x_angle, float y_angle, float x_pos, float y_pos, float z_pos);
void CameraSide(Camera* camera, float speed);
void CameraRotate(float *angle, float speed);
void CameraForward(Camera* camera, float speed);


#endif /* CAMEA_H */
