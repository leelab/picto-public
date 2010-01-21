#include "DDrawObject.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QtScript>
#include <QtNetwork>

static HRESULT PASCAL
EnumFunction(LPDIRECTDRAWSURFACE pSurface,
             LPDDSURFACEDESC, //lpSurfaceDesc
             LPVOID  lpContext)
{
    static BOOL bCalled = FALSE;

    if (!bCalled) {

        *((LPDIRECTDRAWSURFACE *)lpContext) = pSurface;
        bCalled = TRUE;
        return DDENUMRET_OK;
    }
    else {
        pSurface->Release();
        return DDENUMRET_CANCEL;
    }
}

DDrawObject::DDrawObject(QWidget *parent) :
  pDD(NULL),
  pDDSPrimary(NULL),
  pDDSBack(NULL),
  bActive(FALSE),
  positionX(NULL),
  positionY(NULL),
  scriptEngine(),
  QWidget(parent)
{
    DDSURFACEDESC               ddsd;
    HRESULT                     hRet;

	// We need to be a window to initialize DirectDraw
	setWindowFlags(Qt::Window);
/*
    DEVMODE devMode;
    devMode.dmSize = sizeof(DEVMODE);
    
    printf("Enumerating supported display modes\n");
    DWORD modeIndex = 0;    
    while(EnumDisplaySettings(NULL, modeIndex, &devMode))
    {
        printf("Mode %d: %dx%dx%d @ %dHz\n",modeIndex,devMode.dmPelsWidth,devMode.dmPelsHeight,devMode.dmBitsPerPel,devMode.dmDisplayFrequency);
        modeIndex++;
    }

    printf("\nAttempting to switch to 800x600\n");    
    devMode.dmSize = sizeof(DEVMODE);
    devMode.dmPelsWidth = 800;
    devMode.dmPelsHeight = 600;
    devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;   

    switch(ChangeDisplaySettingsEx(NULL, &devMode, NULL, 0, NULL))
    {
        case DISP_CHANGE_SUCCESSFUL:
            printf("DISP_CHANGE_SUCCESSFUL\n");
            break;
        case DISP_CHANGE_BADFLAGS:
            printf("DISP_CHANGE_BADFLAGS\n");
            break;
        case DISP_CHANGE_BADMODE:
            printf("DISP_CHANGE_BADMODE\n");
            break;
        case DISP_CHANGE_BADPARAM:
            printf("DISP_CHANGE_BADPARAM\n");
            break;
        case DISP_CHANGE_FAILED:
            printf("DISP_CHANGE_FAILED\n");
            break;
        case DISP_CHANGE_NOTUPDATED:
            printf("DISP_CHANGE_NOTUPDATED\n");
            break;
        case DISP_CHANGE_RESTART:
            printf("DISP_CHANGE_RESTART\n");
            break;
        default:
            printf("error\n");
            break;
    }
*/
	QRect screenRect = QApplication::desktop()->screenGeometry(-1);
	setGeometry(screenRect.left(), screenRect.top(), screenRect.right(), screenRect.bottom());

	show();

    // Create the main DirectDraw object
    hRet = DirectDrawCreate(NULL, &pDD, NULL);
    if (hRet != DD_OK)
	{
		if(hRet == DDERR_DIRECTDRAWALREADYCREATED)
			printf("DirectDrawCreate FAILED, DirectDraw already created");
		if(hRet == DDERR_GENERIC )
			printf("DirectDrawCreate FAILED, undefined error condition");
		if(hRet == DDERR_INVALIDPARAMS)
			printf("DirectDrawCreate FAILED, Invalid parameters");
		if(hRet == DDERR_OUTOFMEMORY )
			printf("DirectDrawCreate FAILED, Out of memory");

		delete this;
		return;
	}

	// Get exclusive mode
    hRet = pDD->SetCooperativeLevel((HWND) winId(), DDSCL_FULLSCREEN);
    if (hRet != DD_OK)
	{
		delete this;
		return;
	}
/*
    hRet = pDD->SetDisplayMode(800,600,32,60,0);
    if (hRet != DD_OK)
	{
		delete this;
		return;
	}  
*/
    // Create the primary surface with 1 back buffer
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                          DDSCAPS_FLIP;
    ddsd.dwBackBufferCount = 1;
    hRet = pDD->CreateSurface(&ddsd, &pDDSPrimary, NULL);
    if (hRet != DD_OK)
    {
		delete this;
		return;
    }

    // Get a pointer to the back buffer
    hRet = pDDSPrimary->EnumAttachedSurfaces(&pDDSBack, EnumFunction);
    if (hRet != DD_OK)
	{
		delete this;
		return;
	}

// Take exclusive control of the mouse and keyboard, if present, and turn off the mouse cursor
	setFocusPolicy(Qt::StrongFocus);
	grabKeyboard();
	grabMouse(Qt::BlankCursor);

	// Export variables to the scripting engine
	positionX = new QScriptValue(&scriptEngine, 0);
	positionY = new QScriptValue(&scriptEngine, 0);
	circleRadius = new QScriptValue(&scriptEngine, 50);
	elapsedTime = new QScriptValue(&scriptEngine, 0);
	scriptEngine.globalObject().setProperty("positionX", *positionX);
	scriptEngine.globalObject().setProperty("positionY", *positionY);
	scriptEngine.globalObject().setProperty("circleRadius", *circleRadius);
	scriptEngine.globalObject().setProperty("elapsedTime", *elapsedTime);

	// set the default position script
	positionScript = "positionX += 5; positionY+=5; positionX=positionX%1280; positionY=positionY%1024;";
	//random walk
	//positionScript = "positionX += 10 * (Math.random() - 0.5); positionY+=10 * (Math.random() - 0.5); positionX=positionX%1280; positionY=positionY%1024;";
	//Circular Motion
	//positionX = 640 + Math.cos(elapsedTime / 2000) * (512 - circleRadius); positionY = 512 + Math.sin(elapsedTime / 2000) * (512 - circleRadius); positionX = positionX % 1280; positionY = positionY % 1024;
	//Growing and Shrinking
	//circleRadius = 10 + (1 + Math.cos(elapsedTime / 1000)) * 50;

	// Create a timer to flip the surface
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(flipBuffer()));
	timer->start(10);

	// Mark the zero point of our scene time
	sceneTime.start();
}

DDrawObject::~DDrawObject()
{
	releaseMouse();
	releaseKeyboard();

	if(positionX != NULL) delete positionX;
	if(positionY != NULL) delete positionY;
	if(circleRadius != NULL) delete circleRadius;
	if(elapsedTime != NULL) delete elapsedTime;

    if (pDDSBack != NULL)
    {
        pDDSBack->Release();
        pDDSBack = NULL;
    }
    if (pDDSPrimary != NULL)
    {
        pDDSPrimary->Release();
        pDDSPrimary = NULL;
    }
    if (pDD != NULL)
    {
        pDD->Release();
        pDD = NULL;
    }
}

void DDrawObject::setScript(QString newPositionScript)
{
	positionScript = newPositionScript;
}

void DDrawObject::keyPressEvent(QKeyEvent * keyEvent)
{
	if(keyEvent->key() == Qt::Key_Escape)
	{
		QApplication::exit(0);
	}
}

void DDrawObject::flipBuffer()
{
	static bool bProcessingFrame = false;

	if(bProcessingFrame) return;

	bProcessingFrame = true;

    HRESULT hRet;

	UpdateFrame();

	Sleep(0); //process messages

	while(TRUE)
	{
		//hRet = pDDSPrimary->Flip(NULL, 0);
		hRet = pDDSPrimary->Flip(NULL, DDFLIP_WAITVSYNC);		
		if (hRet == DD_OK)
		{
			timeDelta = sceneTime.elapsed() - lastFrameTime;
			lastFrameTime = sceneTime.elapsed();
			break;
		}
		if (hRet == DDERR_SURFACELOST)
		{
			hRet = pDDSPrimary->Restore();
			if (hRet != DD_OK)
				break;
		}
		if (hRet != DDERR_WASSTILLDRAWING)
			break;
	}

	bProcessingFrame = false;
}

void DDrawObject::UpdateFrame()
{
    HDC hdc;
    DDBLTFX ddbltfx;

	*elapsedTime = scriptEngine.toScriptValue(sceneTime.elapsed());
	scriptEngine.globalObject().setProperty("elapsedTime", *elapsedTime);

	scriptEngine.evaluate(positionScript);

	int x, y, radius;
	x = scriptEngine.globalObject().property("positionX").toInteger();
	y = scriptEngine.globalObject().property("positionY").toInteger();
	radius = scriptEngine.globalObject().property("circleRadius").toInteger();

    // Use the blter to do a color fill to clear the back buffer
    memset(&ddbltfx, 0, sizeof(ddbltfx));
    ddbltfx.dwSize = sizeof(ddbltfx);
    ddbltfx.dwFillColor = 0;
    pDDSBack->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAITNOTBUSY, &ddbltfx);

    if(pDDSBack->GetDC(&hdc) == DD_OK)
    {
		SelectObject(hdc,GetStockObject(WHITE_BRUSH));
		SelectObject(hdc,GetStockObject(WHITE_PEN));

		Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);
/*
		WCHAR szRate[1000];

		wsprintf(szRate, L"ms per frame: %d",timeDelta);

		RECT textRect;

		textRect.left = 10;
		textRect.top = 10;
		textRect.right = 300;
		textRect.bottom = 200;

		DrawText(hdc, &szRate[0], -1, &textRect, DT_LEFT | DT_NOCLIP);
*/
        pDDSBack->ReleaseDC(hdc);
    }
}
