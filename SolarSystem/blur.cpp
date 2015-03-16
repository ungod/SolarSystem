#include "shader.h"

GLuint      BlurTexture; 

GLuint texW = 256,	//�����
	   texH = 256;	//�����

//ortho�۲�
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

//perspective�۲�
void ViewPerspective()											
{
	glMatrixMode( GL_PROJECTION );								
	glPopMatrix();												
	glMatrixMode( GL_MODELVIEW );								
	glPopMatrix();												
}

//����������
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


//��������Ⱦ������blurtexture��
void RenderToTexture(void(*drawFunc)(void))
{
	glViewport(0,0,texW,texH);//��С�ֱ��ʣ���Ӧ����
										
	drawFunc();

	//ѡ������BlurTexture
	glBindTexture(GL_TEXTURE_2D,BlurTexture);

	//���Ƴ���������
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, texW, texH, 0);

	//�������
	glClearColor(0.0f, 0.0f, 0.0f, 0.5);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	//��ԭ
	glViewport(0 , 0,512 ,512);
}

//��Ⱦģ��Ч��
void DrawBlur(int times, float inc)	
{
	float spost = 0.0f;				// ����������ʼ���
	float alphainc = 0.9f / times;	// alpha˥���ٶ�
	float alpha = 0.2f;				// ��ʼalpha

	// ȡ���Զ���������
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	//��ʼ��Ⱦblur
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

		spost += inc;//������������λ��
		alpha = alpha - alphainc;//�����Сalpha
	}
	glEnd();							

	ViewPerspective();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D,0);
}