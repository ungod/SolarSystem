#ifndef __texture_included__
#define __texture_included__

#define	Texture_HiQuality		2
#define Texture_MeQuality		1
#define Texture_LoQuality		0
#define Texture_Interpolation	1
#define Texture_Near			0

#include <windows.h>
#include <stdio.h>
#include <gl\gl.h>	
#include <gl\glu.h>	
#include <gl\glaux.h>
#include <gl\glut.h>

/*
texture文件读取
某用原因重复了
*/

extern GLuint		motionBlur_Texture;
extern int motionBlur_Texture_X,
motionBlur_Texture_Y,
motionBlur_Texture_channels,
motionBlur_Texture_type;
extern float	motionBlur_Red_intensity ,
motionBlur_Green_intensity ,
motionBlur_Blue_intensity ,
motionBlur_Alpha_intensity;
extern bool	motionBlur_FirstRenderTexture;

extern int width, height;

AUX_RGBImageRec *LoadBMP(char *);				
int LoadTexture(GLuint &, char *);				
int GenerateTexture(GLuint &, int, int, int, 
					int, int, int);				
int GenerateTexture(GLuint &, int, int, int, 
					int, int, int, unsigned int *);	

#endif