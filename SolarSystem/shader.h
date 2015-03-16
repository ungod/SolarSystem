#ifndef __SHADER_H__
#define __SHADER_H__

#define GLEW_STATIC 1


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "texture.h"
#include "BmpLoader.h"

GLint readShaderSource(char *fileName, GLchar **vertexShader, GLchar **fragmentShader);
GLuint initShader(const GLchar* vertexShader, const GLchar* fragmentShader);
void initUniform1();
void initUniform2();

void RenderToTexture(void(*drawFunc)(void));
void DrawBlur(int times, float inc);
GLuint EmptyTexture();

void ViewOrtho();
void ViewPerspective();	

extern GLuint hFrameBuffer;
extern GLuint fbo;

#endif
