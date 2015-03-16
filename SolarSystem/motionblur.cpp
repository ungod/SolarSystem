#include "motionblur.h"

int CreateMotionBlurTexture(int quality, int interpolation) {	//创建motion blur纹理
	int filter;

	switch(quality) {
		case Texture_HiQuality:
			motionBlur_Texture_X = 512;
			motionBlur_Texture_Y = 512;
			
			motionBlur_Texture_channels = 3;
			motionBlur_Texture_type = GL_RGB;
			break;
		case Texture_MeQuality:
			motionBlur_Texture_X = 256;
			motionBlur_Texture_Y = 256;
			
			motionBlur_Texture_channels = 3;
			motionBlur_Texture_type = GL_RGB;
			break;
		case Texture_LoQuality:
			motionBlur_Texture_X = 128;
			motionBlur_Texture_Y = 128;
			
			motionBlur_Texture_channels = 3;
			motionBlur_Texture_type = GL_RGB;
			break;
	}

	switch(interpolation) {
		case Texture_Interpolation :
			filter = GL_LINEAR;
			break;
		case Texture_Near:
			filter = GL_NEAREST;
			break;
	}

	return GenerateTexture(motionBlur_Texture, 
		motionBlur_Texture_X, 
		motionBlur_Texture_Y,			
		motionBlur_Texture_channels, 
		motionBlur_Texture_type, 
		filter, filter);
}

void RenderToMotionBlurTexture(bool FirstRenderTexture, void SceneFunc()) {

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glLoadIdentity();

	glViewport(0, 0, motionBlur_Texture_X, motionBlur_Texture_Y);

	if(FirstRenderTexture)
		SceneFunc();

	
	glEnable(GL_TEXTURE_2D);
	// 画上次的帧缓冲一次
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, 
			GL_ONE_MINUS_SRC_ALPHA);


		// 自定义纹理颜色强度
		glColor4f(motionBlur_Red_intensity,
				  motionBlur_Green_intensity,
				  motionBlur_Blue_intensity,
				  motionBlur_Alpha_intensity);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

			//画motionblur纹理
			glBindTexture(GL_TEXTURE_2D, motionBlur_Texture);

			glLoadIdentity();
			glOrtho( 0, motionBlur_Texture_X, motionBlur_Texture_Y, 0, 0, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glBegin(GL_QUADS);
				
				glTexCoord2f(0.0, 1.0);  glVertex2f(0, 0);
				glTexCoord2f(0.0, 0.0);  glVertex2f(0, (GLfloat)motionBlur_Texture_Y);
				glTexCoord2f(1.0, 0.0);  glVertex2f((GLfloat)motionBlur_Texture_X, (GLfloat)motionBlur_Texture_Y);
				glTexCoord2f(1.0, 1.0);  glVertex2f((GLfloat)motionBlur_Texture_X, 0);

			glEnd();

			glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);

	glPopMatrix();

	if(!FirstRenderTexture)
		SceneFunc();

	
	// 复制帧缓存到纹理
	glBindTexture(GL_TEXTURE_2D, motionBlur_Texture);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, motionBlur_Texture_type, 0, 0, 
		motionBlur_Texture_X, motionBlur_Texture_Y, 0);

	glViewport(0, 0, width, height);

	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
}

void ShowMotionBlurTexture() {
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

			glBindTexture(GL_TEXTURE_2D, motionBlur_Texture);


			glLoadIdentity();
			glOrtho(0, width, height, 0, 0, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glBegin(GL_QUADS);
				glColor3f(1.0, 1.0, 1.0);

				glTexCoord2f(0.0, 1.0);  glVertex2f(0, 0);
				glTexCoord2f(0.0, 0.0);  glVertex2f(0, (GLfloat)height);
				glTexCoord2f(1.0, 0.0);  glVertex2f((GLfloat)width, (GLfloat)height);
				glTexCoord2f(1.0, 1.0);  glVertex2f((GLfloat)width, 0);

			glEnd();

			glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
}