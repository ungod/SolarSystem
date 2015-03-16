#include "shader.h"
#include "motionblur.h"

#include "CImage.h"
#include <time.h>
#include <GL/glut.h>
#include <math.h>

#define AX_AC	 1
#define AX_DISAC 2
#define AX_QUITE 3
/*
solarSystem v2.0
陈俊光28030201005
2011/6/26
代码实在是乱得要死不好意思
*/

float HourOfDay = 0.0;
float DayOfYear = 0.0;
float AnimateIncrement = 24;
GLUquadric* q;

GLuint hp_sun, hp_earth;
GLuint hSunTex, hStarTex;

bool showAxis = false;
bool isdo = false;

int axState = AX_QUITE;

double xRot = 10;
double yRot = 0;

double ax = 0;
double ay = 0;
double sAx = 0;

double trigger = 0;

GLfloat r = 20;

double starI = 1;
double starA = 0.5;

bool isTrack = false;

void RenderToFbo();
void Render();

//t
int planeRo[5];
int planeSi[5];
int planeSp[5];
int planeTi[5];
float planeRa[5];

//matrix
GLfloat matrixCamera[] = 
{1.0,0.0,0.0,0.0,
 0.0,1.0,0.0,0.0,
 0.0,0.0,1.0,0.0,
 0.0,0.0,0.0,1.0};

GLfloat earthPos[] = 
{1.0,0.0,0.0,0.0,
0.0,1.0,0.0,0.0,
0.0,0.0,1.0,0.0,
0.0,0.0,0.0,1.0};

GLfloat earthRot[] = 
{1.0,0.0,0.0,0.0,
0.0,1.0,0.0,0.0,
0.0,0.0,1.0,0.0,
0.0,0.0,0.0,1.0};

struct texWH
{
	int width;
	int height;
};

#define  NUM_TEXTURES 5

GLuint textureNameArray[NUM_TEXTURES];
const char* fileName[NUM_TEXTURES]={"planet\\mercury.jpg", "planet\\venus.jpg", "planet\\mars.jpg",
"planet\\jupiter.jpg", "planet\\neptune.jpg"
};

//duplicate..
texWH LoadTexture(const char* fileName,bool bMipmap = false)
{
	CImage *img = new CImage;
	img->Load(fileName);
	int width = img->GetWidth();
	int height = img->GetHeight();
	GLubyte *image = new GLubyte[width*height*3];

	for(int j=0; j<height; j++)
	{
		for(int i=0; i<width; i++)
		{
			int index = (j*width+i)*3;
			RGBQUAD rgb = GetPixelColor(img,i,j);
			image[index]=rgb.rgbRed;
			image[index+1]=rgb.rgbGreen;
			image[index+2]=rgb.rgbBlue;
		}
	}
	delete img;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if(bMipmap)
		gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB,width,height,GL_RGB,GL_UNSIGNED_BYTE,image);
	else
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,image);

	delete image;

	texWH tex;
	tex.width = width;
	tex.height = height;

	return tex;
}

void calcPos()
{
	glPushMatrix();
	{
		glLoadIdentity();

		glTranslatef ( 0.0, 0.0, -xRot );
		glRotatef( 30, 1.0, 0.0, 0.0 );

		glRotatef( 360.0*DayOfYear/365.0, 0.0, 1.0, 0.0 );
		glTranslatef( 6.0, 0.0, 0.0 );

		GLfloat matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX,matrix);
		GLfloat mat41 = matrix[12],
			mat42 = matrix[13],
			mat43 = matrix[14]; 

		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX,matrix);
		matrix[12]	= -mat41;
		matrix[13]	= -mat42;
		matrix[14]	= -mat43 - xRot;

		//GLfloat camX,camY,camZ;
		//GLfloat r = 2;
		//camX = matrix[12] + cos(xRot++ * 0.0174)*r;
		//camY = matrix[13];
		//camZ = matrix[14] + sin(yRot++ * 0.0174)*r;
		//gluLookAt(camX, camY, camZ, matrix[12], matrix[13], matrix[14], 0, 1, 0);
		//glGetFloatv(GL_MODELVIEW_MATRIX,matrix);
		//	matrix[12] = camX;
		//	matrix[13] = camY;
		//	matrix[14] = camZ;

		glLoadIdentity();
		glMultMatrixf(matrix);
		glGetFloatv(GL_MODELVIEW_MATRIX,earthPos);
	}
	glPopMatrix();
}


void motionBlurRender()
{
	calcPos();
	RenderToMotionBlurTexture(motionBlur_FirstRenderTexture ,Render);
	ShowMotionBlurTexture();
	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}
void drawAxis()
{
	if (!showAxis) return;
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3f(1,0,0);
	glVertex3f(0,0,0);
	glVertex3f(4,0,0);
	glColor3f(0,1,0);
	glVertex3f(0,0,0);
	glVertex3f(0,4,0);
	glColor3f(0,0,1);
	glVertex3f(0,0,0);
	glVertex3f(0,0,4);
	glEnd();
	glEnable(GL_LIGHTING);
}

void SpecialKeyFunc( int Key, int x, int y )
{

}

void KeyUpFunc(unsigned char Key, int x, int y)
{
	switch ( Key ) {
	case 'W':
	case 'w':
	case 'S':
	case 's':
		axState = AX_DISAC;
		sAx = ax;
		break;
	}
}

void KeyPressFunc( unsigned char Key, int x, int y )
{
	switch ( Key ) {
	case 'R':
	case 'r':
		isdo = !isdo;
		break;
	case 'A':
	case 'a':
		showAxis = !showAxis;
		break;
	case 'u':
	case 'U':
		r++;
		break;
	case 'j':
	case 'J':
		r--;
		break;
	case 's':
	case 'S':
		{	
			if(axState != AX_AC)
			{
				axState = AX_AC;
				trigger = 0.001;
			}
		}
		break;
	case 'w':
	case 'W':
		{	
			if(axState != AX_AC)
			{
				axState = AX_AC;
				trigger = -0.001;
			}
		}
		break;
	case 'c':
	case 'C':
		isTrack = !isTrack;
		break;
	}
}

void initTexture()
{
	BmpLoader sunBmp;
	sunBmp.loadBmp("Sun.bmp");
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &hSunTex);
	glBindTexture(GL_TEXTURE_2D, hSunTex);
	GLubyte* b = (GLubyte*)(sunBmp.mRGB);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, sunBmp.mWidth, sunBmp.mHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, b);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


}

void initTexture2()
{
	BmpLoader sunBmp;
	sunBmp.loadBmp("Stars.bmp");
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &hStarTex);
	glBindTexture(GL_TEXTURE_2D, hStarTex);
	GLubyte* b = (GLubyte*)(sunBmp.mRGB);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, sunBmp.mWidth, sunBmp.mHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, b);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void drawSphere()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hSunTex);
	glEnable(GL_TEXTURE_2D);
	gluSphere(q, 1.5, 15, 15);
	glDisable(GL_TEXTURE_2D);
}

void drawStars(int maxI)
{
	// 取消自动创建纹理
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glEnable(GL_TEXTURE_2D);				 	
	glDisable(GL_DEPTH_TEST);					
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);			
	glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, hStarTex);
	ViewOrtho();	

	for(int i=0;i<maxI;i++)
	{
		glBegin(GL_QUADS);
		glColor4f(1.0f, 1.0f, 1.0f, 0.5);
		glTexCoord2f(0,1);		
		glVertex2f(0,height);

		glTexCoord2f(0,0);
		glVertex2f(0,0);				

		glTexCoord2f(1,0);		
		glVertex2f(width,0);

		glTexCoord2f(1,1);
		glVertex2f(width,height);
		glEnd();
	}

	ViewPerspective();

	//glCopyTexImage2D(GL_TEXTURE_2D, 0, motionBlur_Texture_type, 0, 0, 
		//1000, 600, 0);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D,0);
}

void Render(void)
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(0);

	HourOfDay += AnimateIncrement;
	DayOfYear += AnimateIncrement/24.0;

	HourOfDay = HourOfDay - ((int)(HourOfDay/24))*24;
	DayOfYear = DayOfYear - ((int)(DayOfYear/365))*365;

	glColor3f(0.0, 0.5, 0.5);
	glLoadIdentity();
	//glGetFloatv(GL_MODELVIEW_MATRIX,matrixCamera);

	//gluSphere(q, 2, 15, 15);

	for (int i=0;i<15;i++)
		matrixCamera[i] = earthPos[i];

	if(isTrack)
		glMultMatrixf(matrixCamera);

	//xRot++;
	////glRotatef(xRot, 0, 1, 0);

	//GLfloat matrix[16];
	//glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	//GLfloat camX,camY,camZ;
	//camX = -earthPos[12] + cos(xRot * 0.0174)*r;
	//camY = -earthPos[13];
	//camZ = -earthPos[14] + sin(xRot * 0.0174)*r;
	//static int kk = 0;
	//if(kk>50)
	//	kk = 0;
	//glLoadIdentity();
	//gluLookAt(camX,camY,camZ, -earthPos[12], -earthPos[13], -earthPos[14], 0, 1, 0);

	//glRotatef(xRot++, 0, 1, 0);

	glTranslatef ( 0.0, 0.0, -xRot );

	glRotatef( 30, 1.0, 0.0, 0.0 );


	//太阳
	glPushMatrix();
	{
		glDisable(GL_BLEND);
		static float ys = 0;
		glRotatef(90, 1, 0, 0);
		glRotatef(ys++, 0,0,1);

		RenderToTexture(drawSphere);
		glBindTexture(GL_TEXTURE_2D, hSunTex);
		glEnable(GL_TEXTURE_2D);
		gluSphere(q, 1, 15, 15);
		glDisable(GL_TEXTURE_2D);
		DrawBlur(10,0.01f);

	}
	glPopMatrix();
	drawAxis();

	starI += starA;

	if(starI<1)
		starA = 0.5;
	if(starI >12)
		starA = -0.5;

	drawStars(starI);

	glEnable(GL_TEXTURE_2D);
	for(int i=0;i<1;i++)
	{
		glPushMatrix();
		{
			planeSp[i] += AnimateIncrement/24;
			if(planeSp[i] >= planeTi[i]) planeSp[i] = 0;
			glRotatef( planeSi[i]*(360.0*planeSp[i]/planeTi[i] + planeRo[i]), 0.0, 1.0, 0.0 );
			glTranslatef( 4.0, 0.0, 0.0 );	

			glRotatef(90, 1, 0, 0);
			glRotatef( 360.0*HourOfDay/24.0, 0.0, 0.0, 1.0 );
			glBindTexture(GL_TEXTURE_2D, textureNameArray[i]);
			drawAxis();
			gluSphere(q, 0.6 + planeRa[i], 45, 45);

		}
		glPopMatrix();
	}
	for(int i=1;i<5;i++)
	{
		glPushMatrix();
		{
			planeSp[i] += AnimateIncrement/24;
			if(planeSp[i] >= planeTi[i]) planeSp[i] = 0;
			glRotatef( planeSi[i]*(360.0*planeSp[i]/planeTi[i] + planeRo[i]), 0.0, 1.0, 0.0 );
			glTranslatef( 10.0 + i*7, 0.0, 0.0 );	

			glRotatef(90, 1, 0, 0);
			glRotatef( 360.0*HourOfDay/24.0, 0.0, 0.0, 1.0 );
			drawAxis();
			glBindTexture(GL_TEXTURE_2D, textureNameArray[i]);
			gluSphere(q, 0.6 + planeRa[i]/4.0, 45, 45);

		}
		glPopMatrix();
	}
	glDisable(GL_TEXTURE_2D);

	//地球
	glRotatef( 360.0*DayOfYear/365.0, 0.0, 1.0, 0.0 );
	glTranslatef( 7.0, 0.0, 0.0 );	

	
	glPushMatrix();
	{

		glRotatef(90, 1, 0, 0);
		glRotatef( 360.0*HourOfDay/24.0, 0.0, 0.0, 1.0 );

		//获取未旋转前z轴的，以旋转后坐标轴为参考坐标的矢量
		//角度跟旋转角度相反
		//因为这次修改翻转后原来的代码不适用，关闭
		if(0&&isdo)
			glRotatef(-30, sin((360 - 360.0*DayOfYear/365.0)* 0.017444444), 0, 
					  cos((360 - 360.0*DayOfYear/365.0)*0.017444444));

		drawAxis();

		glUseProgram(hp_earth);
		gluSphere(q, 0.6, 45, 45);
		glUseProgram(0);
		//earthPos[3] += 10;

	}
	glPopMatrix();


	//月球
	glRotatef( 360.0*12.0*DayOfYear/365.0, 0.0, 1.0, 0.0 );
	glTranslatef( 0.7, 0.0, 0.0 );
	glutSolidSphere( 0.1, 5, 5 );

	xRot += ax;
	if(xRot >27)
	{
		xRot = 27;
		ax = 0;
	}
	if(xRot < 2.5)
	{
		xRot = 2.5;
		ax = 0;
		
	}
	if(ax == 0)
		motionBlur_Alpha_intensity = 0.6;

	motionBlur_Alpha_intensity += abs(ax);
	if(motionBlur_Alpha_intensity>0.9)
		motionBlur_Alpha_intensity = 0.9;

	switch(axState)
	{
	case AX_AC:
		{
			ax += trigger;
			if(ax>0.15)
				ax = 0.15;
			if(ax<-0.15)
				ax = -0.15;
		}
		break;
	case AX_DISAC:
		{
			ax -= trigger;
			motionBlur_Alpha_intensity -= abs(ax);
			if(motionBlur_Alpha_intensity<0.6)
				motionBlur_Alpha_intensity = 0.6;
			if(ax*sAx<0)
			{
				ax = 0;
				axState = AX_QUITE;
			}
		}
		break;
	}

}


void OpenGLInit(void)
{
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glClearDepth( 10000.0 );
	glDepthFunc(GL_LEQUAL);	
	glEnable( GL_DEPTH_TEST );
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	GLfloat la[] = {1,1,1,1};
	GLfloat ld[] = {1,1,1,1};
	GLfloat pos[] = {0,0,4,0};
	glLightfv(GL_LIGHT0, GL_AMBIENT, la);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, ld);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	glewInit();

	EmptyTexture();
	GLchar *vs, *fs;

	//太阳着色器
	////readShaderSource("god", &vs, &fs);
	////hp_sun = initShader(vs, fs);
	////initUniform2();

	//地球着色器
	readShaderSource("Earth", &vs, &fs);
	hp_earth = initShader(vs, fs);
	initUniform1();


	q = gluNewQuadric();							
	gluQuadricNormals(q, GL_SMOOTH);				
	gluQuadricTexture(q, GL_TRUE);					

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	initTexture();
	initTexture2();
	 CreateMotionBlurTexture(Texture_HiQuality, Texture_Interpolation);

	 srand(time(NULL));
	 for (int i=0;i<5;i++)
	 {
		planeRo[i] = rand()%360;
		planeSi[i] = rand()%2?-1:1;
		planeSp[i] = 0;
		planeTi[i] = rand()%2000 + 1000;
		planeRa[i] = (float)(rand()%8)/10.0 - (float)(rand()%4)/10.0;
	 }

	 glGenTextures(NUM_TEXTURES,textureNameArray);
	 for(int i=0; i<NUM_TEXTURES; i++){
		 glBindTexture(GL_TEXTURE_2D,textureNameArray[i]);

		 LoadTexture(fileName[i],true);


		 glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		 glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	 }
	 glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
}

void MouseCallback(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON && state ==  GLUT_DOWN)
	{
		static int curX = x;
		static int curY = y;
		
		int relX = curX - x;
		int relY = curY - y;

		if(relX != 0)
		{
			if(relX > 0)
				xRot+= 0.1;
			if(relX < 0)
				xRot-= 0.1;
			curX = x;
		}

	}
}

void ResizeWindow(int w, int h)
{
	float aspectRatio;
	h = (h == 0) ? 1 : h;
	w = (w == 0) ? 1 : w;
	glViewport( 0, 0, w, h );
	aspectRatio = (float)w/(float)h;
	width = w;
	height = h;

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 90.0, aspectRatio, 1.0, 50 );

	glMatrixMode( GL_MODELVIEW );
}
int main( int argc, char** argv )
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( width, height );
	glutCreateWindow( "Solar System - CJG 05" );
	printf("太阳系模拟程序v2.0\n陈俊光 05\n实现了一下技术:\n");

	printf("  1.多重纹理（地球三层纹理）\n  2.太阳了径向模糊和加亮  \n");
	printf("  3.动态模糊 \n  4.纹理叠加和混合  \n");
	printf("  另外优化了操作，带加速和限制  \n");

	printf("\n W/S：拉近/拉远视野，A：打开/关闭坐标， C：切换地球/太阳系视觉  \n");
	//写操作！
	OpenGLInit();

	glutKeyboardFunc( KeyPressFunc );
	glutKeyboardUpFunc(KeyUpFunc);
	glutMouseFunc(MouseCallback);
	glutSpecialFunc( SpecialKeyFunc );

	glutReshapeFunc( ResizeWindow );

	glutDisplayFunc( motionBlurRender );

	glutMainLoop(  );

	return(0);			

}

/*
实验残留品

void calcPos()
{
	glPushMatrix();
	{
		glLoadIdentity();

		glTranslatef ( 0.0, 0.0, -5.0 );
		glRotatef( xRot, 1.0, 0.0, 0.0 );

		glRotatef( 360.0*DayOfYear/365.0, 0.0, 1.0, 0.0 );
		glTranslatef( 6.0, 0.0, 0.0 );

		GLfloat matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX,matrix);
		GLfloat mat41 = matrix[12],
			mat42 = matrix[13],
			mat43 = matrix[14]; 

		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX,matrix);
		matrix[12]	= -mat41;
		matrix[13]	= -mat42;
		matrix[14]	= -mat43 - 5;

		//GLfloat camX,camY,camZ;
		//GLfloat r = 2;
		//camX = matrix[12] + cos(xRot++ * 0.0174)*r;
		//camY = matrix[13];
		//camZ = matrix[14] + sin(yRot++ * 0.0174)*r;
		//gluLookAt(camX, camY, camZ, matrix[12], matrix[13], matrix[14], 0, 1, 0);
		//glGetFloatv(GL_MODELVIEW_MATRIX,matrix);
		//	matrix[12] = camX;
		//	matrix[13] = camY;
		//	matrix[14] = camZ;

		glLoadIdentity();
		glMultMatrixf(matrix);
		glGetFloatv(GL_MODELVIEW_MATRIX,earthPos);
	}
	glPopMatrix();
}

void calcDir()
{
	glPushMatrix();
	{
		glLoadIdentity();
		glMultMatrixf(earthPos);
		glRotatef(xRot--, 0, 1, 0);
		glTranslatef(0, 0, -2);
		GLfloat matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX,earthRot);

	}
	glPopMatrix();
}
*/