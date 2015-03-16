#include "texture.h"

GLuint		motionBlur_Texture;				// The Texture
int motionBlur_Texture_X,					// X Coordonate
motionBlur_Texture_Y,					// Y Coordonate
motionBlur_Texture_channels,			// Number of Chanels
motionBlur_Texture_type;				// The Type
float	motionBlur_Red_intensity = 1.0f,	// Red intensity
motionBlur_Green_intensity = 1.0f,	// Green intensity
motionBlur_Blue_intensity = 1.0f,	// Blue intensity
motionBlur_Alpha_intensity = 0.7f;	// Alpha intensity
bool	motionBlur_FirstRenderTexture = true;	// Do we render the scene first or not?

int width = 1000;
int height = 600;

AUX_RGBImageRec *LoadBMP(char *Filename)				// Loads A Bitmap Image
{
	FILE *File=NULL;									// File Handle

	if (!Filename)										// Make Sure A Filename Was Given
	{
		return NULL;									// If Not Return NULL
	}

	File=fopen(Filename,"r");							// Check To See If The File Exists

	if (File)											// Does The File Exist?
	{
		fclose(File);									// Close The Handle
		return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
	}

	return NULL;										// If Load Failed Return NULL
}

int LoadTexture(GLuint &texture, char *path) 
{
	int Status = FALSE;

	AUX_RGBImageRec *TextureImage = NULL;
	
	//memset(TextureImage,0,sizeof(void *)*1);

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if (TextureImage=LoadBMP(path))
	{
		Status = TRUE;

		// Generate The Texture
		glGenTextures(1, &texture);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture);
		//glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->sizeX, TextureImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage->sizeX, TextureImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering
	}

	if (TextureImage)							// If Texture Exists
	{
		if (TextureImage->data)					// If Texture Image Exists
		{
			free(TextureImage->data);				// Free The Texture Image Memory
		}

		free(TextureImage);						// Free The Image Structure
	}

	return Status;
}

int GenerateTexture(GLuint &Texture, int SizeX, int SizeY, int Channels, int Format, int Min_Filter, int Mag_Filter) {
	int Status = FALSE;
	unsigned int memoryRequiredSize = SizeX * SizeY * Channels;

	unsigned int *memoryBlock = new unsigned int[memoryRequiredSize];
	if(memoryBlock == NULL)
		return Status;

	ZeroMemory(memoryBlock, memoryRequiredSize);

	Status = GenerateTexture(Texture, SizeX, SizeY, Channels, Format, Min_Filter, Mag_Filter, memoryBlock);
	/* -- deprecated, moved into a other procedure
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);

	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

	//gluBuild2DMipmaps(GL_TEXTURE_2D, Channels, SizeX, SizeY, Format, GL_UNSIGNED_INT, memoryBlock);
	glTexImage2D(GL_TEXTURE_2D, 0, Channels, SizeX, SizeY, 0, Format, GL_UNSIGNED_INT, memoryBlock);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Min_Filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Mag_Filter); 
*/
	delete [] memoryBlock;

	int ErrorStatus = glGetError();
	if(ErrorStatus == GL_NO_ERROR)
		Status = TRUE;

	return Status;
}

int GenerateTexture(GLuint &Texture, int SizeX, int SizeY, int Channels, int Format, int Min_Filter, int Mag_Filter, unsigned int *memoryArea) {
	int Status = FALSE;

	if(memoryArea == NULL)
		return Status;

	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);

	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

	//gluBuild2DMipmaps(GL_TEXTURE_2D, Channels, SizeX, SizeY, Format, GL_UNSIGNED_INT, memoryBlock);
	glTexImage2D(GL_TEXTURE_2D, 0, Channels, SizeX, SizeY, 0, Format, GL_UNSIGNED_INT, memoryArea );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Min_Filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Mag_Filter); 


	int ErrorStatus = glGetError();
	if(ErrorStatus == GL_NO_ERROR)
		Status = TRUE;

	return Status;
}
