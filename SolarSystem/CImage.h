#include <windows.h>
#include <atlimage.h>
// 取索引图像的像素索引值。pImgSrc为图像指针，x,y为像素坐标，返回像素的索引值，返回0表示出错
BYTE GetPixelIndex(const CImage* pImgSrc,long x,long y);

// 根据调色板索引值取像素值。pImgSrc为图像指针，idx为索引值，返回RGBQUAD结构的像素值
RGBQUAD GetPaletteColor(const CImage* pImgSrc, BYTE idx);


//取像素颜色值，对索引图像和非索引图像均适用
RGBQUAD GetPixelColor(const CImage* pImgSrc,long x,long y);