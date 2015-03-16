#ifndef __motionblur__included__
#define __motionblur__included__

#include "texture.h"

int CreateMotionBlurTexture(int, int);
void RenderToMotionBlurTexture(bool, void ());
void ShowMotionBlurTexture();

#endif