#include "shader.h"

GLuint      BlurTexture; 

GLuint texW = 256,	//纹理宽
	   texH = 256;	//纹理高

//ortho观察
void ViewOrtho()												
{
	glMatrixMode(GL_PROJECTION);								
	glPushMatrix();											
	glLoadIdentity();										
	glOrtho( 0, width , height , 0, -1, 1 );				
	glMatrixMode(GL_MODELVIEW);								
	glPushMatrix();												
	glLoadIdentity();											
}

//perspective观察
void ViewPerspective()											
{
	glMatrixMode( GL_PROJECTION );								
	glPopMatrix();												
	glMatrixMode( GL_MODELVIEW );								
	glPopMatrix();												
}

//建立空纹理
GLuint EmptyTexture()											
{
	GLuint txtnumber;											
	unsigned int* data;											

	data = (unsigned int*)new GLuint[((texW * texH)* 4 * sizeof(unsigned int))];
	memset(data, 0, ((texW * texH)* 4 * sizeof(unsigned int)));	

	glGenTextures(1, &txtnumber);								
	glBindTexture(GL_TEXTURE_2D, txtnumber);					
	glTexImage2D(GL_TEXTURE_2D, 0, 4, texW, texH, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, data);						
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	delete [] data;												

    BlurTexture = txtnumber;
	return txtnumber;											
}


//将场景渲染到纹理blurtexture中
void RenderToTexture(void(*drawFunc)(void))
{
	glViewport(0,0,texW,texH);//减小分辨率，适应纹理
										
	drawFunc();

	//选择纹理BlurTexture
	glBindTexture(GL_TEXTURE_2D,BlurTexture);

	//复制场景到纹理
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, texW, texH, 0);

	//清除场景
	glClearColor(0.0f, 0.0f, 0.0f, 0.5);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	//复原
	glViewport(0 , 0,512 ,512);
}

//渲染模糊效果
void DrawBlur(int times, float inc)	
{
	float spost = 0.0f;				// 纹理坐标起始间隔
	float alphainc = 0.9f / times;	// alpha衰减速度
	float alpha = 0.2f;				// 初始alpha

	// 取消自动创建纹理
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	//开始渲染blur
	glEnable(GL_TEXTURE_2D);				 	
	glDisable(GL_DEPTH_TEST);					
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);			
	glEnable(GL_BLEND);							
	glBindTexture(GL_TEXTURE_2D, BlurTexture);
	ViewOrtho();								

	alphainc = alpha / times;

	glBegin(GL_QUADS);
	for (int num = 0;num < times;num++)
	{
		glColor4f(1.0f, 1.0f, 1.0f, alpha);
		glTexCoord2f(0+spost,1-spost);		
		glVertex2f(0,0);

		glTexCoord2f(0+spost,0+spost);
		glVertex2f(0,height);				

		glTexCoord2f(1-spost,0+spost);		
		glVertex2f(width,height);

		glTexCoord2f(1-spost,1-spost);
		glVertex2f(width,0);

		spost += inc;//径向扩大纹理位置
		alpha = alpha - alphainc;//径向减小alpha
	}
	glEnd();							

	ViewPerspective();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D,0);
}