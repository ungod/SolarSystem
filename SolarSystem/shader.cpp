#include "shader.h"

#include <fcntl.h>
#include <io.h>
#include <stdlib.h>

typedef enum {
	EVertexShader,
	EFragmentShader,
} EShaderType;

GLuint g_hp;
GLuint hFrameBuffer;

GLuint fbo;        
GLuint depthBuffer;

//shader大小读取
int shaderSize(char *fileName, EShaderType shaderType)
{
	int fd;
	char name[100];
	int count = -1;

	strcpy(name, fileName);

	switch (shaderType)
	{
	case EVertexShader:
		strcat(name, ".vert");
		break;
	case EFragmentShader:
		strcat(name, ".frag");
		break;
	default:
		printf("ERROR: 未知类型 \n");
		exit(1);
		break;
	}

	fd = _open(name, _O_RDONLY);
	if (fd != -1)
	{
		count = _lseek(fd, 0, SEEK_END) + 1;
		_close(fd);
	}

	return count;
}

//shader文件读取
int readShader(char *fileName, EShaderType shaderType, char *shaderText, int size)
{
	FILE *fh;
	char name[100];
	int count;

	strcpy(name, fileName);

	switch (shaderType) 
	{
	case EVertexShader:
		strcat(name, ".vert");
		break;
	case EFragmentShader:
		strcat(name, ".frag");
		break;
	default:
		printf("ERROR: 未知类型 \n");
		exit(1);
		break;
	}

	fh = fopen(name, "r");
	if (!fh)
		return -1;

	fseek(fh, 0, SEEK_SET);
	count = (int) fread(shaderText, 1, size, fh);
	shaderText[count] = '\0';

	if (ferror(fh))
		count = 0;

	fclose(fh);
	return count;
}

//将文件读取到内存
GLint readShaderSource(char *fileName, GLchar **vertexShader, GLchar **fragmentShader)
{
	int vSize, fSize;

	// 分配内存
	vSize = shaderSize(fileName, EVertexShader);
	fSize = shaderSize(fileName, EFragmentShader);

	if ((vSize == -1) || (fSize == -1))
	{
		printf("Cannot determine size of the shader %s\n", fileName);
		return 0;
	}

	*vertexShader = (GLchar *) malloc(vSize);
	*fragmentShader = (GLchar *) malloc(fSize);


	// 读取源码
	if (!readShader(fileName, EVertexShader, *vertexShader, vSize))
	{
		printf("Cannot read the file %s.vert\n", fileName);
		return 0;
	}

	if (!readShader(fileName, EFragmentShader, *fragmentShader, fSize))
	{
		printf("Cannot read the file %s.frag\n", fileName);
		return 0;
	}

	return 1;
}

GLuint initShader(const GLchar* vertexShader, const GLchar* fragmentShader)
{
	GLuint hVS, hFS, hProg;				//着色器和着色程序句柄
	GLint  vertCompiled, fragCompiled;  //编译状态
	GLint  linked;
	GLuint hTex_1, hTex_2;				//纹理句柄

	//创建顶点和片元着色
	hVS = glCreateShader(GL_VERTEX_SHADER);
	hFS = glCreateShader(GL_FRAGMENT_SHADER);

	//读取数据
	glShaderSource(hVS, 1, &vertexShader, NULL);
	glShaderSource(hFS, 1, &fragmentShader, NULL);

	//编译
	glCompileShader(hVS);
	glGetShaderiv(hVS, GL_COMPILE_STATUS, &vertCompiled);
	glCompileShader(hFS);
	glGetShaderiv(hFS, GL_COMPILE_STATUS, &fragCompiled);

	//错误返回
	if (!vertCompiled || !fragCompiled)
		return 0;

	//创建着色程序并连接着色器
	hProg = glCreateProgram();
	glAttachShader(hProg, hVS);
	glAttachShader(hProg, hFS);

	//链接着色程序
	glLinkProgram(hProg);
	glGetProgramiv(hProg, GL_LINK_STATUS, &linked);

	if (!linked)
		return 0;

	glUseProgram(hProg);
	g_hp = hProg;

	return hProg;
}

void initUniform1()
{
	GLuint hTex_1, hTex_2, hTex_3;				//纹理句柄

	//读取bmp
	BmpLoader bmp_1, bmp_2, bmp_3;
	bmp_1.loadBmp("Day.bmp");
	bmp_2.loadBmp("Night.bmp");
	bmp_3.loadBmp("Clouds.bmp");

	GLubyte* texPtr = (GLubyte*)bmp_1.mRGB;

	//纹理层0
	glActiveTexture(GL_TEXTURE1);

	//创建纹理层并设置对应参数
	glGenTextures(1, &hTex_1);
	glBindTexture(GL_TEXTURE_2D, hTex_1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, bmp_1.mWidth, bmp_1.mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texPtr);

	
	////纹理层1
	glActiveTexture(GL_TEXTURE2);
	texPtr = (GLubyte*)bmp_2.mRGB;
	glGenTextures(1, &hTex_2);
	glBindTexture(GL_TEXTURE_2D, hTex_2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp_2.mWidth, bmp_2.mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texPtr);

	//纹理层2
	glActiveTexture(GL_TEXTURE3);
	texPtr = (GLubyte*)bmp_3.mRGB;
	glGenTextures(1, &hTex_3);
	glBindTexture(GL_TEXTURE_2D, hTex_3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp_3.mWidth, bmp_3.mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texPtr);

	//glPushMatrix();
	//glLoadIdentity();
	//glTranslatef ( 0.0, 0.0, -10.0 );
	//glRotatef( 30.0, 1.0, 0.0, 0.0 );
	//GLfloat matrix[16];
	//glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	//glPopMatrix();
	//设置着色器uniform的值
	glUniform3f(glGetUniformLocation(g_hp, "LightPosition"), 0.0f, 0.0f, -8.0f);
	glUniform1i(glGetUniformLocation(g_hp, "EarthDay"), 1);
	glUniform1i(glGetUniformLocation(g_hp, "EarthNight"), 2);
	glUniform1i(glGetUniformLocation(g_hp, "EarthCloudGloss"), 3);

	//返回纹理0
	glActiveTexture(GL_TEXTURE0);
}


//
//以下代码为实验残留品
//不起作用
//

void initFBO()
{
	//创建fbo
	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);//为framebuffer绑定fbo

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	//创建渲染对象rbo
	glGenRenderbuffersEXT(1, &depthBuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);//为framebuffer绑定渲染对象rbo
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, viewport[2], viewport[3]);

	//绑定到纹理让着色器使用它
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, hFrameBuffer, 0);   

	//绑定rbo到fbo    
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer); 

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);   
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT)   
		exit(1);   

	//暂时关闭fbo
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}

void initUniform2()
{

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &hFrameBuffer);
	glBindTexture(GL_TEXTURE_2D, hFrameBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, viewport[2], viewport[3], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//glUniform1f(glGetUniformLocation(g_hp, "FrameWidth"), viewport[2]);
	//glUniform1f(glGetUniformLocation(g_hp, "FrameHeight"), viewport[3]);
	//glUniform1f(glGetUniformLocation(g_hp, "textureWidth"), viewport[2]);
	//glUniform1f(glGetUniformLocation(g_hp, "textureHeight"), viewport[3]);
	//glUniform1f(glGetUniformLocation(g_hp, "Frequency"), 5);
	//glUniform1f(glGetUniformLocation(g_hp, "Offset"), 0);
	//glUniform1f(glGetUniformLocation(g_hp, "Fade"), 0.1);
	//glUniform1i(glGetUniformLocation(g_hp, "Speed"), 1);
	//glUniform1iARB(glGetUniformLocation(g_hp, "FrameBuffer"), 0);
	

	//glUniform1f(glGetUniformLocation(g_hp, "m_Size"), 200000);
	glUniform1iARB(glGetUniformLocation(g_hp, "RayCastSceneTex"), 0);
	glUniform2f(glGetUniformLocation(g_hp, "RayPoint"), 0, 0.5);
	glUniform1i(glGetUniformLocation(g_hp, "SampleNum"), 10);
	glUniform1f(glGetUniformLocation(g_hp, "RayCastFactor"), 100);
	glUniform1f(glGetUniformLocation(g_hp, "RayExposure"), 200000);
	glUniform4f(glGetUniformLocation(g_hp, "RayColor"), 0, 0, 1, 1);

	initFBO();
}
