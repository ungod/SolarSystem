#include "CImage.h"


// 取索引图像的像素索引值。pImgSrc为图像指针，x,y为像素坐标，返回像素的索引值，返回0表示出错
BYTE GetPixelIndex(const CImage* pImgSrc,long x,long y)
{
	if ((pImgSrc == NULL)||(pImgSrc->IsIndexed() == false)) return 0;// 图像指针为空或不是索引图像则返回0

	if ((x < 0)||(y < 0)||(x >= pImgSrc->GetWidth())||(y >= pImgSrc->GetHeight())) //判断像素座标合法性
		return 0;

	BYTE* pDst = (BYTE*)pImgSrc->GetPixelAddress(x,y);//取像素值地址
	if (pImgSrc->GetBPP() == 8){	//索引值用8位表示
		return *pDst;
	} else {
		BYTE pos;
		BYTE iDst = *pDst;
		if (pImgSrc->GetBPP() == 4){//索引值用4位表示
			pos = (BYTE)(4 * (1 - x % 2));
			iDst &= (0x0F << pos);
			return (BYTE)(iDst >> pos);
		} else if (pImgSrc->GetBPP() == 1){//索引值用1位表示(二值图像)
			pos = (BYTE)(7 - x % 8);
			iDst &= (0x01 << pos);
			return (BYTE)(iDst >> pos);
		}
	}
	return 0;
}

// 根据调色板索引值取像素值。pImgSrc为图像指针，idx为索引值，返回RGBQUAD结构的像素值
RGBQUAD GetPaletteColor(const CImage* pImgSrc, BYTE idx)
{
	RGBQUAD rgb = {0,0,0,0};
	if ((pImgSrc)&&(pImgSrc->IsIndexed())){
		int ncolors = pImgSrc->GetMaxColorTableEntries();//取调色板颜色数目
		RGBQUAD *prgbColors = new RGBQUAD[ncolors]; 
		pImgSrc->GetColorTable(0,ncolors,prgbColors);//取调色板
		if (idx < ncolors)
			rgb = prgbColors[idx];//得到像素值
		delete [] prgbColors;
	}	
	return rgb;
}

//取像素颜色值，对索引图像和非索引图像均适用
RGBQUAD GetPixelColor(const CImage* pImgSrc,long x,long y)
{
	RGBQUAD rgb={0,0,0,0};
	if ((!pImgSrc)||(x < 0)||(y < 0)||(x >= pImgSrc->GetWidth())||(y >= pImgSrc->GetHeight()))
		return rgb;

	if (pImgSrc->IsIndexed()){//索引图像
		rgb = GetPaletteColor(pImgSrc,GetPixelIndex(pImgSrc,x,y));
	} else {//非索引图像
		BYTE* iDst  = (BYTE*)pImgSrc->GetPixelAddress(x,y);
		rgb.rgbBlue = *iDst++;
		rgb.rgbGreen= *iDst++;
		rgb.rgbRed  = *iDst++;
		if(pImgSrc->GetBPP() > 24)
			rgb.rgbReserved = *iDst;
	}
	return rgb;
}