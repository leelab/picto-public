#include "DDrawVisualTarget.h"

#include "DDrawCompositingSurface.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>


namespace Picto {

/*! Gets called once for every attached surface (not including the primary one).
 *	This is used to find the surface we are using as aback buffer.  Since there
 *	is only a single surface attached to the primary sruface, if this gets
 *	called more than once, we'll throw an error
 */
static HRESULT PASCAL enumFunction(LPDIRECTDRAWSURFACE pSurface, LPDDSURFACEDESC lpSurfaceDesc, LPVOID  lpContext)
{
	static bool called = false;
	if(called)
	{
		QString informativeText = "If you are seeing this message, something has gone ";
		informativeText += "horribly wrong.  Try restarting the PictoBox.";
		informativeText += "Error: enumFunction - more than one surface attached";

		QMessageBox errorMsgBox;
		errorMsgBox.setText("DirectDrawVisualTarget Failed to initialize");
		errorMsgBox.setInformativeText(informativeText);
		errorMsgBox.exec();

		QCoreApplication::exit(1);
		//this return is pointless, since we quit the app, but without
		//it, there is a path through the function with no return
		return DDENUMRET_CANCEL;
	}
	else
	{
		called = true;
		*((LPDIRECTDRAWSURFACE *)lpContext) = pSurface;
		return DDENUMRET_OK;
	}

}


DDrawVisualTarget::DDrawVisualTarget(int _width, int _height) :
	VisualTarget(false, _width, _height)
{
	HWND hWnd;
	//set full-screen mode
	QWidget::setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

	QRect screenRect = QApplication::desktop()->screenGeometry(-1);
	width_ = screenRect.width();
	height_ = screenRect.height();
	resize(width_,height_);
	move(0,0);

	setAttribute(Qt::WA_NativeWindow,true);
	hWnd = (HWND)winId();
	setAttribute(Qt::WA_PaintOnScreen,true);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);

	show();

	//Set up DirectDraw
	directDraw_ = 0;
	primarySurface_ = 0;
	backSurface_ = 0;
	

	HRESULT hRet;
	hRet = DirectDrawCreate(NULL, &directDraw_, NULL);
	if (hRet != DD_OK)
	{
		if(hRet == DDERR_DIRECTDRAWALREADYCREATED)
			initFail("DirectDrawCreate FAILED, DirectDraw already created");
		if(hRet == DDERR_GENERIC )
			initFail("DirectDrawCreate FAILED, undefined error condition");
		if(hRet == DDERR_INVALIDPARAMS)
			initFail("DirectDrawCreate FAILED, Invalid parameters");
		if(hRet == DDERR_OUTOFMEMORY )
			initFail("DirectDrawCreate FAILED, Out of memory");
		//initFail(QString("DirectDrawCreate FAILED, code=%1").arg(HRESULT_CODE(hRet)));
	}

    // Get exclusive mode
    hRet = directDraw_->SetCooperativeLevel(hWnd, DDSCL_FULLSCREEN);
    if (hRet != DD_OK)
        initFail("SetCooperativeLevel FAILED");


    // Create the primary surface with 1 back buffer

    DDSURFACEDESC ddsd;
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_VIDEOMEMORY;
    ddsd.dwBackBufferCount = 1;

    hRet = directDraw_->CreateSurface(&ddsd, &primarySurface_, NULL);
    if (hRet != DD_OK)
    {
        if (hRet == DDERR_NOFLIPHW)
			initFail("CreateSurface FAILED: Display driver doesn't support flipping surfaces.");
        if (hRet == DDERR_INCOMPATIBLEPRIMARY)
			initFail("CreateSurface FAILED: DDERR_INCOMPATIBLEPRIMARY");
        if (hRet == DDERR_INVALIDCAPS)
			initFail("CreateSurface FAILED: DDERR_INVALIDCAPS");
        if (hRet == DDERR_INVALIDOBJECT)
			initFail("CreateSurface FAILED: DDERR_INVALIDOBJECT");
        if (hRet == DDERR_INVALIDPARAMS)
			initFail("CreateSurface FAILED: DDERR_INVALIDPARAMS");
        if (hRet == DDERR_INVALIDPIXELFORMAT)
			initFail("CreateSurface FAILED: DDERR_INVALIDPIXELFORMAT");
        if (hRet == DDERR_NOALPHAHW)
			initFail("CreateSurface FAILED: DDERR_NOALPHAHW");
        if (hRet == DDERR_NOCOOPERATIVELEVELSET)
			initFail("CreateSurface FAILED: DDERR_NOCOOPERATIVELEVELSET");
        if (hRet == DDERR_NOOVERLAYHW)
			initFail("CreateSurface FAILED: DDERR_NOOVERLAYHW");
        if (hRet == DDERR_OUTOFMEMORY)
			initFail("CreateSurface FAILED: DDERR_OUTOFMEMORY");
        if (hRet == DDERR_OUTOFVIDEOMEMORY)
			initFail("CreateSurface FAILED: DDERR_OUTOFVIDEOMEMORY");
        if (hRet == DDERR_PRIMARYSURFACEALREADYEXISTS)
			initFail("CreateSurface FAILED: DDERR_PRIMARYSURFACEALREADYEXISTS");
  
        initFail("CreateSurface FAILED");
    }
    // Get a pointer to the back buffer
	//This requires a bit of explanation.  EnumAttachedSurfaces calls enumFunction
	//for every attached surface (only one in this case).  enumFunction then
	//sets the backSurface_ pointer.
	hRet = primarySurface_->EnumAttachedSurfaces(&backSurface_, enumFunction);
    if (hRet != DD_OK)
        initFail("EnumAttachedSurfaces FAILED");

	//Clear both buffers (this seems redundant, since the buffers get redrawn every frame, 
	//but if we don't do it, the first frame may contain old image data
	DDBLTFX clearFX;
    memset(&clearFX, 0, sizeof(clearFX));
    clearFX.dwSize = sizeof(clearFX);
    clearFX.dwFillColor = 0;
    backSurface_->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAITNOTBUSY, &clearFX);
    primarySurface_->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAITNOTBUSY, &clearFX);

	//Check the device capabilities for color keying
	DDCAPS devCaps;
	memset(&devCaps,0,sizeof(devCaps));
	directDraw_->GetCaps(&devCaps,0);

	printf("Getting device color key capabilities...\n");
	if(devCaps.dwCKeyCaps == DDCKEYCAPS_BOTHBLT)
		printf("DDCKEYCAPS_BOTHBLT\n");
	if(devCaps.dwCKeyCaps == DDCKEYCAPS_DESTBLT)
		printf("DDCKEYCAPS_DESTBLT\n");
	if(devCaps.dwCKeyCaps == DDCKEYCAPS_DESTBLTCLRSPACE)
		printf("DDCKEYCAPS_DESTBLTCLRSPACE\n");
	if(devCaps.dwCKeyCaps == DDCKEYCAPS_DESTBLTCLRSPACEYUV)
		printf("DDCKEYCAPS_DESTBLTCLRSPACEYUV\n");
	if(devCaps.dwCKeyCaps == DDCKEYCAPS_SRCBLT)
		printf("DDCKEYCAPS_SRCBLT\n");
	if(devCaps.dwCKeyCaps == DDCKEYCAPS_SRCBLTCLRSPACE)
		printf("DDCKEYCAPS_SRCBLTCLRSPACE\n");
	if(devCaps.dwCKeyCaps == DDCKEYCAPS_SRCBLTCLRSPACEYUV)
		printf("DDCKEYCAPS_SRCBLTCLRSPACEYUV\n");


}

DDrawVisualTarget::~DDrawVisualTarget()
{
	if(directDraw_)
	{
		directDraw_->Release();
		directDraw_ = 0;
	}
	if(primarySurface_)
	{
		primarySurface_->Release();
		primarySurface_ = 0;
	}
	if(backSurface_)
	{
		backSurface_->Release();
		backSurface_ = 0;
	}

}


//outputs an error message if something goes wrong with DirectDraw
void DDrawVisualTarget::initFail(QString errorMsg)
{
	QString informativeText = "If you are seeing this message, something has gone ";
	informativeText += "horribly wrong.  Try restarting the PictoBox.\n\n";
	informativeText += "Error: " + errorMsg;

	QMessageBox errorMsgBox;
	errorMsgBox.setText("DirectDrawVisualTarget Failed to initialize");
	errorMsgBox.setInformativeText(informativeText);
	errorMsgBox.exec();

	QCoreApplication::exit(1);
}

QSharedPointer<CompositingSurface> DDrawVisualTarget::generateCompositingSurface()
{
	QSharedPointer<DDrawCompositingSurface> ddrawCompositingSurface(new DDrawCompositingSurface(directDraw_));

	return ddrawCompositingSurface;

}

QString DDrawVisualTarget::getTypeName()
{
	return QString("DirectDraw");
}

void DDrawVisualTarget::draw(QPoint location, QSharedPointer<CompositingSurface> compositingSurface)
{
	if(compositingSurface->getTypeName()=="DirectDraw")
	{
		HRESULT hRet;

		//grab the surface
		LPDIRECTDRAWSURFACE sourceSurf = compositingSurface.staticCast<DDrawCompositingSurface>()->getSurface();
		
		DDSURFACEDESC sourceDesc;
		memset(&sourceDesc, 0, sizeof(sourceDesc));
		sourceDesc.dwSize = sizeof(sourceDesc);
		hRet = sourceSurf->GetSurfaceDesc(&sourceDesc);

		if(hRet != DD_OK)
		{
			if(hRet == DDERR_INVALIDOBJECT)
				printf("DDERR_INVALIDOBJECT\n");
			if(hRet == DDERR_INVALIDPARAMS)
				printf("DDERR_INVALIDPARAMS\n");\
			printf("GetSurfaceDesc failed \n");
		}

		//create the source and destination rectangles
		RECT srcRect;
		srcRect.left = 0;
		srcRect.top = 0;
		//srcRect.right = 100;
		srcRect.right = sourceDesc.dwWidth;
		//srcRect.bottom = 70;
		srcRect.bottom = sourceDesc.dwHeight;

		RECT destRect;
		destRect.left = location.x();
		destRect.top = location.y();
		destRect.right = destRect.left + (srcRect.right - srcRect.left);
		destRect.bottom = destRect.top + (srcRect.bottom - srcRect.top);


		//set up the alpha blending effects
		DDALPHABLTFX alphaFX;
		memset(&alphaFX, 0, sizeof(alphaFX));
		alphaFX.dwSize = sizeof(alphaFX);

		//blit whatever we're drawing to the back buffer
	
		//The moreMagic flag needs to be explained.  For some unknown reason, the AlphaBlt
		//function does not seem to work unless we tell it that the source rectangle is using 
		//inverted alpha values.  I have no idea why this is the case, or what is causing
		//it, but the VIA video drivers seem like a liekly culprit (since the DirectDraw
		//examples never use AlphaBlt, this could have been missed during testing).
		//Refer to http://catb.org/jargon/html/magic-story.html for an explanation of "moreMagic"
		int moreMagic = DDABLT_ALPHASRCNEG;


		//Try using the blitfx struct to define the color key
		DDCOLORKEY colorKey;
		colorKey.dwColorSpaceHighValue = 0;
		colorKey.dwColorSpaceLowValue = 0;

		DDBLTFX blitFX;
		memset(&blitFX, 0, sizeof(blitFX));
		blitFX.dwSize = sizeof(blitFX);
		blitFX.ddckSrcColorkey = colorKey;

		LARGE_INTEGER tick, tock;
		float elapsedTime;
		LARGE_INTEGER ticksPerSec;
		QueryPerformanceFrequency(&ticksPerSec);

		QueryPerformanceCounter(&tick);
		//hRet = backSurface_->AlphaBlt(&destRect, sourceSurf, &srcRect, moreMagic, &alphaFX);
		//hRet = backSurface_->Blt(&destRect, sourceSurf, &srcRect,0,0);
		//hRet = backSurface_->Blt(&destRect, sourceSurf, &srcRect, DDBLT_WAITNOTBUSY | DDBLT_KEYSRC,NULL);
		hRet = backSurface_->Blt(&destRect, sourceSurf, &srcRect, DDBLT_WAITNOTBUSY | DDBLT_KEYSRCOVERRIDE,&blitFX);
		QueryPerformanceCounter(&tock);
		elapsedTime = (double)(tock.LowPart-tick.LowPart)/(double)(ticksPerSec.LowPart);
		printf("Blt took: %f seconds\n", elapsedTime);
		
		if(hRet != DD_OK)
		{
			if(hRet == DDERR_GENERIC)
				printf("DDERR_GENERIC\n");
			if(hRet == DDERR_INVALIDOBJECT)
				printf("DDERR_INVALIDOBJECT\n");
			if(hRet == DDERR_INVALIDPARAMS)
				printf("DDERR_INVALIDPARAMS\n");
			if(hRet == DDERR_INVALIDRECT)
				printf("DDERR_INVALIDRECT\n");
			if(hRet == DDERR_NOALPHAHW)
				printf("DDERR_NOALPHAHW\n");
			if(hRet == DDERR_NOBLTHW)
				printf("DDERR_NOBLTHW\n");
			if(hRet == DDERR_NOPALETTEATTACHED)
				printf("DDERR_NOPALETTEATTACHED\n");
			if(hRet == DDERR_NOSTRETCHHW)
				printf("DDERR_NOSTRETCHHW\n");
			if(hRet == DDERR_OVERLAPPINGRECTS)
				printf("DDERR_OVERLAPPINGRECTS\n");
			if(hRet == DDERR_SURFACEBUSY)
				printf("DDERR_SURFACEBUSY\n");
			if(hRet == DDERR_SURFACELOST)
				printf("DDERR_SURFACELOST\n");
			if(hRet == DDERR_UNSUPPORTED)
				printf("DDERR_UNSUPPORTED\n");

			printf("DDrawVisualTarget::draw() blitting failed\n");
		}




		//Let's try alpha blending with the GDI functions
		//This works, but I'm concerned that using a GDI function defeats 
		//the purpose of DirectDraw...
		//If this turns out to be too slow, we can write our own AlphaBlitter.
		//It should also be noted that this works without having to define a 
		//custom pixel format with a seperate alpha bitmask.
		/*HDC destDC;
		HDC srcDC;
		backSurface_->GetDC(&destDC);
		sourceSurf->GetDC(&srcDC);

		SetStretchBltMode(destDC, BILINEAR);
		SetStretchBltMode(srcDC, BILINEAR);

		BLENDFUNCTION blendFn;
		memset(&blendFn, 0, sizeof(blendFn));
		blendFn.BlendOp = AC_SRC_OVER;
		blendFn.BlendFlags = 0;
		blendFn.SourceConstantAlpha = 10;
		blendFn.AlphaFormat = AC_SRC_ALPHA;

		printf("source rectangle: (%d, %d, %d, %d)\n", 0,0,sourceDesc.dwWidth,sourceDesc.dwHeight);
		printf("dest rectangle: (%d, %d, %d, %d)\n", location.x(), location.y(),sourceDesc.dwWidth,sourceDesc.dwHeight);

		AlphaBlend(destDC, location.x(), location.y(),sourceDesc.dwWidth,sourceDesc.dwHeight,
			srcDC,0,0,sourceDesc.dwWidth,sourceDesc.dwHeight,blendFn);
		//AlphaBlend(destDC, location.x(), location.y(),101,100,
			//srcDC,0,0,100,100,blendFn);

		backSurface_->ReleaseDC(destDC);
		sourceSurf->ReleaseDC(srcDC);*/

	}
}

void DDrawVisualTarget::present()
{
	HRESULT hRet;

    while (1)
    {
        hRet = primarySurface_->Flip(NULL, 0);
        if (hRet == DD_OK)
            break;
        if (hRet == DDERR_SURFACELOST)
        {
            hRet = primarySurface_->Restore();
            if (hRet != DD_OK)
                break;
        }
        if (hRet != DDERR_WASSTILLDRAWING)
            break;
    }

	//clear the backbuffer
    // Use bltfx to do a color fill to clear the back buffer
	DDBLTFX clearFX;
    memset(&clearFX, 0, sizeof(clearFX));
    clearFX.dwSize = sizeof(clearFX);
    clearFX.dwFillColor = 0;
    backSurface_->Blt(NULL,	NULL, NULL, DDBLT_COLORFILL | DDBLT_WAITNOTBUSY, &clearFX);

}

void DDrawVisualTarget::paintEvent(QPaintEvent *)
{
	//I'm not entirely sure how to do a paint event here...
}


void DDrawVisualTarget::drawNonExperimentText(QFont font, QColor color, QRect rect, Qt::AlignmentFlag alignment, QString text)
{
	//I could implement this function by getting the device context of the
	//back surface (backSurface_->getDC) and then using the standard Windows 
	//drawing calls.  This is kind of painful since the passed in values are
	//all QT values, so I would need to convert the font/color/rectangle to
	//Windows values.

	//However, I think I'm not going to do that for the moment, and simply leave
	//this function unimplemented
}






}; //namespace Picto
