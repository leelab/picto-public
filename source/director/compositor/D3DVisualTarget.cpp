#include "D3DVisualTarget.h"
#include "D3DCompositingSurface.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>

#include <tchar.h>


namespace Picto {



D3DVisualTarget::D3DVisualTarget() :
	VisualTarget(false, 800,600)
{
	//zero all of the pointers
	pD3D_ = 0;  
	pD3dDevice_ = 0;
	pVertexBuffer_ = 0;

	HWND hWnd;

	//If we don't put in the line below.  All mouse values will be
	//about 25 pixels off of their screen locations since Qt will
	//take the invisible window frame into account when computing
	//mouse position.
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


	//----------------------------------
	//Set up Direct3D 
	//----------------------------------
    // Create the D3D object, which is needed to create the D3DDevice.
	pD3D_ = Direct3DCreate9(D3D_SDK_VERSION);
	if(!pD3D_)
		d3dFail("D3DVisualTarget: Direct3DCreate FAILED");





	D3DDISPLAYMODE        DisplayMode;
	pD3D_->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &DisplayMode);


	// Set up the presentation parameters
    D3DPRESENT_PARAMETERS d3dpp; 
    memset( &d3dpp, 0, sizeof(d3dpp) );
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferFormat = DisplayMode.Format;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.Windowed = false; 
	d3dpp.BackBufferHeight = DisplayMode.Height;
	d3dpp.BackBufferWidth = DisplayMode.Width;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	HRESULT hr;

	hr = pD3D_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,&d3dpp, &pD3dDevice_);
	
	if(hr)
	{
		d3dFail("D3DVisualTarget: CreateDevice FAILED");
		printf("D3DVisualTarget: CreateDevice FAILED %x\n", hr);
	}

    // Turn off D3D lighting
    pD3dDevice_->SetRenderState( D3DRS_LIGHTING, FALSE );

    // Turn on alpha blending
	//NOTE: at various stages during development, we had to use inverse alpha values
	//so it's possible that if alpha blending isn't working, that may be a cause
    pD3dDevice_->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pD3dDevice_->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pD3dDevice_->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	//get device capabilities
	D3DCAPS9 caps;
    hr = pD3dDevice_->GetDeviceCaps(&caps);
	if(hr)
		d3dFail("D3DVisualTarget: GetDeviceCaps FAILED");

	//----------------------------------
	//Set up the Vertex Stream
	//----------------------------------
	//Note that the texture y-coordinates are flipped.  This is because Picto coordinates
	//start in the top right corner, while D3D coordinates start in the bottom left.
	CUSTOMVERTEX quadVertexData[] =
	{
	  { 0.0f, 0.0f, 0.0f, D3DCOLOR_ARGB(0, 255, 255, 255), 0.0f, 1.0f },
	  { 0.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(0, 255, 255, 255), 0.0f, 0.0f },
	  { 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(0, 255, 255, 255), 1.0f, 0.0f },
	  { 0.0f, 0.0f, 0.0f, D3DCOLOR_ARGB(0, 255, 255, 255), 0.0f, 1.0f },
	  { 1.0f, 1.0f, 0.0f, D3DCOLOR_ARGB(0, 255, 255, 255), 1.0f, 0.0f },
	  { 1.0f, 0.0f, 0.0f, D3DCOLOR_ARGB(0, 255, 255, 255), 1.0f, 1.0f },
	};

    // Create the vertex buffer.
    hr = pD3dDevice_->CreateVertexBuffer(sizeof(quadVertexData), 0,
											D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,
											D3DPOOL_DEFAULT, &pVertexBuffer_, NULL);
	if(hr)
		d3dFail("D3DVisualTarget: CreateVertexBuffer FAILED");

    // Fill the vertex buffer.
    void* pVertices;
    hr = pVertexBuffer_->Lock(0, 0, &pVertices, 0);
	if(hr)
		d3dFail("D3DVisualTarget: pVertexBuffer_->Lock FAILED");

	memcpy(pVertices, quadVertexData, sizeof(quadVertexData));
    pVertexBuffer_->Unlock();

	//Traditionally, the stream source would be set at the beginning of rendering each frame
	//However to save CPU cycles (and since the rendering is kind of trivial), we'll
	//set the Stream source here).
	pD3dDevice_->SetStreamSource(0, pVertexBuffer_, 0, sizeof(CUSTOMVERTEX));
	pD3dDevice_->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 );


	//----------------------------------
	//Set up Matrices
	//----------------------------------
	// Set up our view matrix as an identity matrix
	D3DXMATRIX matView;
	D3DXMatrixIdentity( &matView );
	pD3dDevice_->SetTransform( D3DTS_VIEW, (D3DMATRIX*)&matView);

	// For the projection matrix, we use a simple orthogonal matrix
	// Basic Orthographic transform.
	// Left   = 0  Right = width_
	// Bottom = 0  Top   = height_
	// Near   = -1 Far   = 1
	D3DXMATRIX matProj;
	D3DXMatrixOrthoOffCenterLH(&matProj,0.0f,(float)width_,0.0f,(float)height_,-1.0f,1.0f);
	pD3dDevice_->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)&(matProj*matView));

	//The world matrix changes for each sprite, so we deal with that while rendering...

	//----------------------------------
	//Increase thread priority
	//----------------------------------
	//Since this visual target is only going to be used in Win32, we
	//can bump the thread priority to the top.  Not
	HANDLE hProcess, hThread;
	hProcess = GetCurrentProcess();
	hThread = GetCurrentThread();
	//SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS);
	SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS);

	//Doing the following makes the mouse non-responsive.
	//This is fine when using an eye-tracker input, but unacceptable
	//when testing
	//SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS);

	//SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
	SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);

}
void D3DVisualTarget::closeEvent(QCloseEvent *event)
 {
	 Q_UNUSED(event);
	if(pD3dDevice_) 
	{
		pD3dDevice_->Release();
	}

	if(pVertexBuffer_)
	{
        pVertexBuffer_->Release();
	}
    if(pD3D_)
	{
        pD3D_->Release();
	}

	//return the thread back to normal priority
	HANDLE hProcess, hThread;
	hProcess = GetCurrentProcess();
	hThread = GetCurrentThread();
	SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS);
	SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
}

D3DVisualTarget::~D3DVisualTarget()
{	
}

//! Return a D3D Compositing surface
QSharedPointer<CompositingSurface> D3DVisualTarget::generateCompositingSurface()
{
	QSharedPointer<D3DCompositingSurface> d3dCompositingSurface(new D3DCompositingSurface(pD3dDevice_));

	return d3dCompositingSurface;
}

QString D3DVisualTarget::getTypeName()
{
	return QString("Direct3D");
}

void D3DVisualTarget::draw(QPoint location, QSharedPointer<CompositingSurface> compositingSurface)
{
	if(compositingSurface->getTypeName() == "Direct3D")
	{
		//retreive a pointer to the texture
		LPDIRECT3DTEXTURE9 tex = compositingSurface.staticCast<D3DCompositingSurface>()->getTexture();

		//add texture pointer to the rendering list
		textureList_.push_back(tex);

		D3DSURFACE_DESC texDesc;
		tex->GetLevelDesc(0, &texDesc);

		//figure out the position for the sprite, and add it to the list
		//NOTE: Picto uses the standard screen coodinates with (0,0) in the top left
		//corner.  However, the D3D setup we are using has (0,0) in the bottom left corner, so
		//we need to find the "complement" of the y-coordinate.
		//The value stored in the positionList_ is the bottom left corner (inclusive)
		//of the sprite.
		QPointF quadPoint;
		quadPoint.setX(location.x());
		quadPoint.setY((height_-location.y()-(int)texDesc.Height));
		positionList_.push_back(quadPoint);
	}
	else
	{
		d3dFail("D3DVisualTarget: draw called with a compositing surface not of type \"Direct3D\"");
	}
}


void D3DVisualTarget::present()
{
	LPDIRECT3DTEXTURE9 tex;
	QPointF quadPoint;
	D3DXMATRIX matScale, matTrans;
	D3DSURFACE_DESC texDesc;

	HRESULT hr;


	//begin the scene
	hr = pD3dDevice_->BeginScene();
    if(hr)
		d3dFail("D3DVisualTarget: BeginScene FAILED");

	//render each textured quad
	while(!textureList_.isEmpty())
	{
		tex = textureList_.takeFirst();
		quadPoint = positionList_.takeFirst();

		tex->GetLevelDesc(0, &texDesc);

		//The 0.5 offset on the translation matrix is done to ensure perfect texture mapping
		D3DXMatrixScaling(&matScale, (float)texDesc.Width, (float)texDesc.Height, 0.0f);
		D3DXMatrixTranslation(&matTrans, quadPoint.x()-0.5f, quadPoint.y()-0.5f, 0.0f);
		pD3dDevice_->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&(matScale*matTrans));

        pD3dDevice_->SetTexture( 0, tex );
		pD3dDevice_->DrawPrimitive(D3DPT_TRIANGLELIST,
											0,		//Start Vertex
											2);		//Primitive Count
	}

	//end the scene
	hr = pD3dDevice_->EndScene();
    if(hr)
		d3dFail("D3DVisualTarget: EndScene FAILED");

	//Don't call present until we are in vertical blanking
	//BYTE outBuf;
	//DWORD bytesReturned;
		//DeviceIoControl(hCRT,IOCTRL_WAIT_VBLANK_START,NULL,0,&outBuf,1,&bytesReturned,NULL);
	
	//Wait For Vertical Blank
	D3DRASTER_STATUS rastStat;
	hr = pD3dDevice_->GetRasterStatus(0,&rastStat);
	while(!hr && !rastStat.InVBlank)
	{
		hr = pD3dDevice_->GetRasterStatus(0,&rastStat);
	}
	if(hr)
		d3dFail("D3DVisualTarget: Monitor does not support reading of the current scan line.");

	static int test = 0;

	//present the back buffer
    hr = pD3dDevice_->Present( NULL, NULL, NULL, NULL );
    if(hr)
		d3dFail("D3DVisualTarget: Present FAILED");

	//Wait for rastering to begin
	while(!hr && rastStat.InVBlank)
	{
		hr = pD3dDevice_->GetRasterStatus(0,&rastStat);
	}
	setFirstPhosphorTime();

	//clear the back buffer
    pD3dDevice_->Clear( 0, NULL, D3DCLEAR_TARGET,0, 0.0f, 0 );

	emit presented();

}

//!We'll never get any paint events since we run in full screen
void D3DVisualTarget::paint(QPaintDevice *widget)
{
	Q_UNUSED(widget);
}

/*!	\Brief Draws text on the screen
 *
 *	WARNING: This function is slow, and will likely result in dropped frames.
 */
void D3DVisualTarget::drawNonExperimentText(QFont font, QColor color, QRect rect, Qt::AlignmentFlag alignment, QString text)
{
	ID3DXFont *d3dFont;
	HRESULT hr;

	//Get the FontInfo from the QFont so we can figure out what font
	//we should use
	QFontInfo qFontInfo(font);

	//Set up and create the font
	D3DXFONT_DESC fontDesc;
	memset(&fontDesc, 0, sizeof(fontDesc));
	fontDesc.Height = qFontInfo.pixelSize();
	fontDesc.Width = 0;
	if(qFontInfo.bold())
		fontDesc.Weight = FW_BOLD;
	else
		fontDesc.Weight = FW_NORMAL;
	fontDesc.MipLevels = 0;
	fontDesc.Italic = qFontInfo.italic();
	fontDesc.CharSet = DEFAULT_CHARSET;  //This limits our ability to internationalize...
	fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	fontDesc.Quality = DEFAULT_QUALITY;
	fontDesc.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	wchar_t fontName[32];
	qFontInfo.family().toWCharArray(fontName);
	wcscpy_s(fontDesc.FaceName,32, fontName);

	hr = D3DXCreateFontIndirect(pD3dDevice_, &fontDesc, &d3dFont);
	if(hr)
		d3dFail("D3DVisualTarget D3DXCreateFontIndirect FAILED");

	//set up all the stuff we need to draw text
	wchar_t *textW;
	textW = new wchar_t[text.length()];
	text.toWCharArray(textW);

	RECT textRect;
	textRect.bottom = rect.bottom();
	textRect.left = rect.left();
	textRect.right = rect.right();

	DWORD textFormat = 0;
	if(alignment & Qt::AlignLeft)
		textFormat |= DT_LEFT;
	if(alignment & Qt::AlignRight)
		textFormat |= DT_RIGHT;
	if(alignment & Qt::AlignHCenter)
		textFormat |= DT_CENTER;
	if(alignment & Qt::AlignTop)
		textFormat |= DT_TOP;
	if(alignment & Qt::AlignBottom)
		textFormat |= DT_BOTTOM;
	if(alignment & Qt::AlignVCenter)
		textFormat |= DT_VCENTER;

	D3DCOLOR textColor = D3DCOLOR_ARGB(color.alpha(),color.red(), color.green(), color.blue());
	

	pD3dDevice_->BeginScene();

	textRect.top = rect.top();

	hr = d3dFont->DrawText(NULL,textW,text.length(),&textRect,textFormat,textColor);
	
	pD3dDevice_->EndScene();

	d3dFont->Release();


}



//outputs an error message if something goes wrong with Direct3DMobile
void D3DVisualTarget::d3dFail(QString errorMsg)
{
	QString text = "If you are seeing this message, something has gone ";
	text += "horribly wrong.  Try restarting the PictoBox.\n\n";
	text += "Error: " + errorMsg;

	QMessageBox errorMsgBox;
	errorMsgBox.setText(text);
	errorMsgBox.exec();

	QCoreApplication::exit(1);
}

void D3DVisualTarget::clear()
{
	//Since the back buffer was cleared the last time we presented, 
	//I can simply flip it to the front, and we will be done
    pD3dDevice_->Present( NULL, NULL, NULL, NULL );

}


};