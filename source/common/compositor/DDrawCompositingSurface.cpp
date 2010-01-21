#include <QImage>

#include "DDrawCompositingSurface.h"

namespace Picto {

DDrawCompositingSurface::DDrawCompositingSurface(LPDIRECTDRAW _dDraw)
{
	surface_ = 0;
	dDraw_ = _dDraw;
}

DDrawCompositingSurface::~DDrawCompositingSurface()
{
	if(surface_)
	{
		surface_->Release();
		surface_ = 0;
	}
}

//convertImage takes the passed in QImage and converts it to a DirectDraw
//surface, which is then stored.
void DDrawCompositingSurface::convertImage(QImage image)
{
	//If we already have a valid surface, we need to clean it up and 
	//start over.  Maybe as this develops, I can reuse an existing surface?
	if(surface_)
	{
		surface_->Release();
		surface_ = 0;
	}
	
	//Create a DirectDrawSurface

	DDSURFACEDESC ddsd;
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;

	ddsd.dwHeight = image.height(); 
	ddsd.dwWidth = image.width(); 

	HRESULT hRet;

	hRet = dDraw_->CreateSurface(&ddsd,&surface_,NULL);
	if (hRet != DD_OK)
    {
        if (hRet == DDERR_NOFLIPHW)
			printf("CreateSurface FAILED: Display driver doesn't support flipping surfaces.\n");
        if (hRet == DDERR_INCOMPATIBLEPRIMARY)
			printf("CreateSurface FAILED: DDERR_INCOMPATIBLEPRIMARY\n");
        if (hRet == DDERR_INVALIDCAPS)
			printf("CreateSurface FAILED: DDERR_INVALIDCAPS\n");
        if (hRet == DDERR_INVALIDOBJECT)
			printf("CreateSurface FAILED: DDERR_INVALIDOBJECT\n");
        if (hRet == DDERR_INVALIDPARAMS)
			printf("CreateSurface FAILED: DDERR_INVALIDPARAMS\n");
        if (hRet == DDERR_INVALIDPIXELFORMAT)
			printf("CreateSurface FAILED: DDERR_INVALIDPIXELFORMAT\n");
        if (hRet == DDERR_NOALPHAHW)
			printf("CreateSurface FAILED: DDERR_NOALPHAHW\n");
        if (hRet == DDERR_NOCOOPERATIVELEVELSET)
			printf("CreateSurface FAILED: DDERR_NOCOOPERATIVELEVELSET\n");
        if (hRet == DDERR_NOOVERLAYHW)
			printf("CreateSurface FAILED: DDERR_NOOVERLAYHW\n");
        if (hRet == DDERR_OUTOFMEMORY)
			printf("CreateSurface FAILED: DDERR_OUTOFMEMORY\n");
        if (hRet == DDERR_OUTOFVIDEOMEMORY)
			printf("CreateSurface FAILED: DDERR_OUTOFVIDEOMEMORY\n");
        if (hRet == DDERR_PRIMARYSURFACEALREADYEXISTS)
			printf("CreateSurface FAILED: DDERR_PRIMARYSURFACEALREADYEXISTS\n");
  
		printf("DDrawCompositingSurface::convertImage(): CreateSurface failed\n");
		Q_ASSERT(false);
    }

	DDCOLORKEY colorKey;
	colorKey.dwColorSpaceHighValue = 0;
	colorKey.dwColorSpaceLowValue = 0;
	surface_->SetColorKey(DDCKEY_SRCBLT, &colorKey);


	//if needed, convert the image format
	if(image.format() != QImage::Format_ARGB32)
	{
		printf("converting image\n");
		image = image.convertToFormat(QImage::Format_ARGB32);
	}

	//copy the image to the surface
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	hRet = surface_->Lock(NULL,&ddsd,DDLOCK_DISCARD | DDLOCK_WRITEONLY,NULL);
	if(hRet != DD_OK)
	{
		if(hRet == DDERR_INVALIDOBJECT)
			printf("DDERR_INVALIDOBJECT\n");
		if(hRet == DDERR_INVALIDPARAMS)
			printf("DDERR_INVALIDPARAMS\n");
		if(hRet == DDERR_OUTOFMEMORY)
			printf("DDERR_OUTOFMEMORY\n");
		if(hRet == DDERR_SURFACEBUSY)
			printf("DDERR_SURFACEBUSY\n");
		if(hRet == DDERR_SURFACELOST)
			printf("DDERR_SURFACELOST\n");
		if(hRet == DDERR_WASSTILLDRAWING)
			printf("DDERR_WASSTILLDRAWING\n");

		printf("Locking surface failed\n");
		Q_ASSERT(false);
	}
	UCHAR *destFirstPixel = (UCHAR*)ddsd.lpSurface;
	DWORD destYPitch = ddsd.lPitch;
	DWORD destXPitch = ddsd.lXPitch;

	QRgb* srcPixel = (QRgb*)image.bits();

	for(int y=0; y<image.height(); y++)
	{
		for(int x=0; x<image.width(); x++)
		{
			*(DWORD*)(destFirstPixel + destYPitch*y + destXPitch*x) = (DWORD)(*srcPixel);
			if(y == 0 && x<5)
				printf("Source pixel = 0x%08X\n", (DWORD)(*srcPixel));
			srcPixel++;
		}
	}

	surface_->Unlock(NULL);
}

QString DDrawCompositingSurface::getTypeName()
{
	return QString("DirectDraw");
}

LPDIRECTDRAWSURFACE DDrawCompositingSurface::getSurface()
{
	return surface_;
}

}; //namespace Picto
