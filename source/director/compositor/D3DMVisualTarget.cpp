#include "D3DMVisualTarget.h"

#include "D3DMCompositingSurface.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>


//WARNING!!!!!

//After a whole bunch of debugging, we came to the following conclusion:

	//When rendering on PictoBox (EPIA-M700) with VIA drivers, the rendering
	//is totally, and completely screwed up.

//In exclusive mode, the present function isn't actually flipping buffers, so 
//all rendering gets done on the front surface.  This causes serious display issues

//In windowed mode, there is a significant tear line in the middle of the screen
//Additionally, the first few (~25) frames render at high speed (~500 us), and the 
//remaining frames render 10 times slower (~5 ms).  This is not due to the buffer
//filling up, since waiting for vsync doesn't solve the problem

//As such, this code is being abandoned (along with Windows CE) and all rendering
//will be done in Direct3D (under Windows XP).  I have attempted to leave the code 
//in what I believe would be a functioning state, if the VIA drivers worked as advertised...

namespace Picto {

////////////////////////////////////////////
//This is the windowed mode constructor.
////////////////////////////////////////////


/*!
 *	The constructor takes care of initializing Direct3D Mobile, and setting
 *	up everything that will be needed for rendering textured quads.  In a
 *	traditional Direct3D application, we would break this up into multiple
 *	functions (intiD3DM, initGeomtry, initMatrices, etc).  Given the simplicity 
 *	of our setup (textured quads), all of this is combined into a single function.
 */
//D3DMVisualTarget::D3DMVisualTarget() :
//	VisualTarget(false, 640,480)
//{
//	HWND hWnd;
//
//	//Set full-screen mode
//	//Direct3DMobile had trouble on PictoBox when we tried to run it in exclusive
//	//mode.  So, this is full-screen, but it is still technically a window
//	QWidget::setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
//
//	QRect screenRect = QApplication::desktop()->screenGeometry(-1);
//	width_ = screenRect.width();
//	height_ = screenRect.height();
//	resize(width_,height_);
//	move(0,0);
//
//	setAttribute(Qt::WA_NativeWindow,true);
//	hWnd = (HWND)winId();
//	setAttribute(Qt::WA_PaintOnScreen,true);
//
//    QPalette pal = palette();
//    pal.setColor(QPalette::Window, Qt::black);
//    setPalette(pal);
//
//	show();
//
//	//Set up the CRT "file" so we can wait for vertical blanking to call present
//	hCRT = CreateFile(_T("CRT1:"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,0);
//
//
//	//----------------------------------
//	//Set up Direct3D Mobile
//	//----------------------------------
//    // Create the D3DM object, which is needed to create the D3DMDevice.
//    pD3DM_ = Direct3DMobileCreate(D3DM_SDK_VERSION);
//	if(!pD3DM_)
//		d3dFail("D3DMVisualTarget: Direct3DMobileCreate FAILED");
//
//	D3DMDISPLAYMODE        DisplayMode;
//	pD3DM_->GetAdapterDisplayMode(D3DMADAPTER_DEFAULT, &DisplayMode);
//
//
//	// Set up the presentation parameters
//	//NOTE: D3DM was seriously unhappy in exclusive mode, so instead we're
//	//using windowed mode.  Change this setting at your own risk...
//    D3DMPRESENT_PARAMETERS d3dmpp; 
//    memset( &d3dmpp, 0, sizeof(d3dmpp) );
//	d3dmpp.BackBufferFormat = DisplayMode.Format;
//	d3dmpp.BackBufferCount = 1;
//	d3dmpp.MultiSampleType = D3DMMULTISAMPLE_NONE;
//	d3dmpp.SwapEffect = D3DMSWAPEFFECT_DISCARD;
//	d3dmpp.Windowed = true; 
//
//	HRESULT hr;
//	hr = pD3DM_->CreateDevice(D3DMADAPTER_DEFAULT, D3DMDEVTYPE_DEFAULT, hWnd, 0,&d3dmpp, &pD3dmDevice_);
//	if(hr)
//		d3dFail("D3DMVisualTarget: CreateDevice FAILED");
//
//    // Turn off D3D lighting
//    pD3dmDevice_->SetRenderState( D3DMRS_LIGHTING, FALSE );
//
//    // Turn on alpha blending
//	//NOTE: at various stages during development, we had to use inverse alpha values
//	//so it's possible that if alpha blending isn't working, that may be a cause
//    pD3dmDevice_->SetRenderState( D3DMRS_ALPHABLENDENABLE, TRUE );
//	pD3dmDevice_->SetRenderState(D3DMRS_SRCBLEND,D3DMBLEND_SRCALPHA);
//	pD3dmDevice_->SetRenderState(D3DMRS_DESTBLEND,D3DMBLEND_INVSRCALPHA);
//
//	//get device capabilities
//	D3DMCAPS caps;
//    hr = pD3dmDevice_->GetDeviceCaps(&caps);
//	if(hr)
//		d3dFail("D3DMVisualTarget: GetDeviceCaps FAILED");
//
//	//----------------------------------
//	//Set up the Vertex Stream
//	//----------------------------------
//	CUSTOMVERTEX quadVertexData[] =
//	{
//	  { 0.0f, 0.0f, 0.0f, D3DMCOLOR_ARGB(0, 255, 255, 255), 0.0f, 0.0f },
//	  { 0.0f, 1.0f, 0.0f, D3DMCOLOR_ARGB(0, 255, 255, 255), 0.0f, 1.0f },
//	  { 1.0f, 1.0f, 0.0f, D3DMCOLOR_ARGB(0, 255, 255, 255), 1.0f, 1.0f },
//	  { 1.0f, 0.0f, 0.0f, D3DMCOLOR_ARGB(0, 255, 255, 255), 1.0f, 0.0f },
//	};
//
//    D3DMPOOL pool;
//    if (caps.SurfaceCaps & D3DMSURFCAPS_VIDVERTEXBUFFER)
//        pool = D3DMPOOL_VIDEOMEM;
//    else
//        pool = D3DMPOOL_SYSTEMMEM;
//
//    // Create the vertex buffer.
//    hr = pD3dmDevice_->CreateVertexBuffer(sizeof(quadVertexData), 0,
//											D3DMFVF_XYZ_FLOAT | D3DMFVF_DIFFUSE | D3DMFVF_TEX1,
//											pool, &pVertexBuffer_);
//	if(hr)
//		d3dFail("D3DMVisualTarget: CreateVertexBuffer FAILED");
//
//    // Fill the vertex buffer.
//    void* pVertices;
//    hr = pVertexBuffer_->Lock(0, 0, &pVertices, 0);
//	if(hr)
//		d3dFail("D3DMVisualTarget: pVertexBuffer_->Lock FAILED");
//
//	memcpy(pVertices, quadVertexData, sizeof(quadVertexData));
//    pVertexBuffer_->Unlock();
//
//	//Traditionally, the stream source would be set at the beginning of rendering each frame
//	//However to save CPU cycles (and since the rendering is kind of trivial), we'll
//	//set the Stream source here).
//	pD3dmDevice_->SetStreamSource(0, pVertexBuffer_, sizeof(CUSTOMVERTEX));
//
//	//----------------------------------
//	//Set up the Index Stream
//	//----------------------------------
//	WORD quadIndexData[] =
//	{
//	  0, 1, 2,
//	  0, 2, 3,
//	};
//
//	//create index buffer
//    if (caps.SurfaceCaps & D3DMSURFCAPS_VIDINDEXBUFFER)
//        pool = D3DMPOOL_VIDEOMEM;
//    else
//        pool = D3DMPOOL_SYSTEMMEM;
//
//	hr = pD3dmDevice_->CreateIndexBuffer(sizeof(quadIndexData), 
//									D3DMUSAGE_LOCKABLE, D3DMFMT_INDEX16,
//									pool, &pIndexBuffer_);
//	if(hr)
//		d3dFail("D3DMVisualTarget: CreateIndexBuffer FAILED");
//
//	//fill index buffer
//	void* pIndices;
//	hr = pIndexBuffer_->Lock( 0, 0, &pIndices, 0);
//	if(hr)
//		d3dFail("D3DMVisualTarget: pIndexBuffer_->Lock FAILED");
//
//	memcpy(pIndices, quadIndexData, sizeof(quadIndexData));
//	pIndexBuffer_->Unlock();
//
//	//again set the index buffer here instead of at the time of rendering
//	pD3dmDevice_->SetIndices(pIndexBuffer_);
//
//	//----------------------------------
//	//Set up Matrices
//	//----------------------------------
//	// Set up our view matrix as an identity matrix
//	D3DMXMATRIX matView;
//	D3DMXMatrixIdentity( &matView );
//	pD3dmDevice_->SetTransform( D3DMTS_VIEW, (D3DMMATRIX*)&matView, D3DMFMT_D3DMVALUE_FLOAT );
//
//	// For the projection matrix, we use a simple orthogonal matrix
//	// Basic Orthographic transform.
//	// Left   = 0  Right = width_
//	// Bottom = 0  Top   = height_
//	// Near   = -1 Far   = 1
//	D3DMXMATRIX matProj;
//	D3DMXMatrixOrthoOffCenterLH(&matProj,0.0f,(float)width_,0.0f,(float)height_,-1.0f,1.0f);
//	pD3dmDevice_->SetTransform( D3DMTS_PROJECTION, (D3DMMATRIX*)&(matProj*matView), D3DMFMT_D3DMVALUE_FLOAT );
//
//	//The world matrix changes for each sprite, so we deal with that while rendering...
//}
//
////! On destruction, release all of the created COM interfaces
//D3DMVisualTarget::~D3DMVisualTarget()
//{
//    if(pVertexBuffer_)        
//        pVertexBuffer_->Release();
//
//	if(pIndexBuffer_)        
//        pIndexBuffer_->Release();
//
//    if(pD3dmDevice_) 
//        pD3dmDevice_->Release();
//
//    if(pD3DM_)
//        pD3DM_->Release();
//
//	CloseHandle(hCRT);
//}






////////////////////////////////////////////
//This is the exclusive mode constructor.
////////////////////////////////////////////



D3DMVisualTarget::D3DMVisualTarget() :
	VisualTarget(false, 640,480)
{
	HWND hWnd;

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

	//Set up the CRT "file" so we can wait for vertical blanking to call present
	hCRT = CreateFile(_T("CRT1:"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,0);


	//----------------------------------
	//Set up Direct3D Mobile
	//----------------------------------
    // Create the D3DM object, which is needed to create the D3DMDevice.
    pD3DM_ = Direct3DMobileCreate(D3DM_SDK_VERSION);
	if(!pD3DM_)
		d3dFail("D3DMVisualTarget: Direct3DMobileCreate FAILED");

	int adapter = D3DMADAPTER_DEFAULT;




	D3DMDISPLAYMODE        DisplayMode;
	pD3DM_->GetAdapterDisplayMode(adapter, &DisplayMode);


	// Set up the presentation parameters
    D3DMPRESENT_PARAMETERS d3dmpp; 
    memset( &d3dmpp, 0, sizeof(d3dmpp) );
	d3dmpp.BackBufferFormat = DisplayMode.Format;
	d3dmpp.BackBufferCount = 1;
	d3dmpp.MultiSampleType = D3DMMULTISAMPLE_NONE;
	d3dmpp.SwapEffect = D3DMSWAPEFFECT_FLIP;
	d3dmpp.Windowed = false; 
	d3dmpp.BackBufferHeight = DisplayMode.Height;
	d3dmpp.BackBufferWidth = DisplayMode.Width;
	d3dmpp.FullScreen_PresentationInterval = D3DMPRESENT_INTERVAL_DEFAULT;
	d3dmpp.Flags = D3DMPRESENTFLAG_LOCKABLE_BACKBUFFER;

	HRESULT hr;
	hr = pD3DM_->CreateDevice(D3DMADAPTER_DEFAULT, D3DMDEVTYPE_DEFAULT, hWnd, 0,&d3dmpp, &pD3dmDevice_);
	
	if(hr)
	{
		d3dFail("D3DMVisualTarget: CreateDevice FAILED");
		printf("D3DMVisualTarget: CreateDevice FAILED %x\n", hr);
	}

    // Turn off D3D lighting
    pD3dmDevice_->SetRenderState( D3DMRS_LIGHTING, FALSE );

    // Turn on alpha blending
	//NOTE: at various stages during development, we had to use inverse alpha values
	//so it's possible that if alpha blending isn't working, that may be a cause
    pD3dmDevice_->SetRenderState( D3DMRS_ALPHABLENDENABLE, TRUE );
	pD3dmDevice_->SetRenderState(D3DMRS_SRCBLEND,D3DMBLEND_SRCALPHA);
	pD3dmDevice_->SetRenderState(D3DMRS_DESTBLEND,D3DMBLEND_INVSRCALPHA);

	//get device capabilities
	D3DMCAPS caps;
    hr = pD3dmDevice_->GetDeviceCaps(&caps);
	if(hr)
		d3dFail("D3DMVisualTarget: GetDeviceCaps FAILED");

	//----------------------------------
	//Set up the Vertex Stream
	//----------------------------------
	CUSTOMVERTEX quadVertexData[] =
	{
	  { 0.0f, 0.0f, 0.0f, D3DMCOLOR_ARGB(0, 255, 255, 255), 0.0f, 0.0f },
	  { 0.0f, 1.0f, 0.0f, D3DMCOLOR_ARGB(0, 255, 255, 255), 0.0f, 1.0f },
	  { 1.0f, 1.0f, 0.0f, D3DMCOLOR_ARGB(0, 255, 255, 255), 1.0f, 1.0f },
	  { 0.0f, 0.0f, 0.0f, D3DMCOLOR_ARGB(0, 255, 255, 255), 0.0f, 0.0f },
	  { 1.0f, 1.0f, 0.0f, D3DMCOLOR_ARGB(0, 255, 255, 255), 1.0f, 1.0f },
	  { 1.0f, 0.0f, 0.0f, D3DMCOLOR_ARGB(0, 255, 255, 255), 1.0f, 0.0f },
	};

    D3DMPOOL pool;
    if (caps.SurfaceCaps & D3DMSURFCAPS_VIDVERTEXBUFFER)
        pool = D3DMPOOL_VIDEOMEM;
    else
        pool = D3DMPOOL_SYSTEMMEM;

    // Create the vertex buffer.
    hr = pD3dmDevice_->CreateVertexBuffer(sizeof(quadVertexData), 0,
											D3DMFVF_XYZ_FLOAT | D3DMFVF_DIFFUSE | D3DMFVF_TEX1,
											pool, &pVertexBuffer_);
	if(hr)
		d3dFail("D3DMVisualTarget: CreateVertexBuffer FAILED");

    // Fill the vertex buffer.
    void* pVertices;
    hr = pVertexBuffer_->Lock(0, 0, &pVertices, 0);
	if(hr)
		d3dFail("D3DMVisualTarget: pVertexBuffer_->Lock FAILED");

	memcpy(pVertices, quadVertexData, sizeof(quadVertexData));
    pVertexBuffer_->Unlock();

	//Traditionally, the stream source would be set at the beginning of rendering each frame
	//However to save CPU cycles (and since the rendering is kind of trivial), we'll
	//set the Stream source here).
	pD3dmDevice_->SetStreamSource(0, pVertexBuffer_, sizeof(CUSTOMVERTEX));


	//----------------------------------
	//Set up Matrices
	//----------------------------------
	// Set up our view matrix as an identity matrix
	D3DMXMATRIX matView;
	D3DMXMatrixIdentity( &matView );
	pD3dmDevice_->SetTransform( D3DMTS_VIEW, (D3DMMATRIX*)&matView, D3DMFMT_D3DMVALUE_FLOAT );

	// For the projection matrix, we use a simple orthogonal matrix
	// Basic Orthographic transform.
	// Left   = 0  Right = width_
	// Bottom = 0  Top   = height_
	// Near   = -1 Far   = 1
	D3DMXMATRIX matProj;
	D3DMXMatrixOrthoOffCenterLH(&matProj,0.0f,(float)width_,0.0f,(float)height_,-1.0f,1.0f);
	pD3dmDevice_->SetTransform( D3DMTS_PROJECTION, (D3DMMATRIX*)&(matProj*matView), D3DMFMT_D3DMVALUE_FLOAT );

	//The world matrix changes for each sprite, so we deal with that while rendering...
}

//! On destruction, release all of the created COM interfaces
D3DMVisualTarget::~D3DMVisualTarget()
{
	//if(pIndexBuffer_)        
	//{
 //       pIndexBuffer_->Release();
	//}

	if(pVertexBuffer_)
	{
        pVertexBuffer_->Release();
	}

    if(pD3dmDevice_) 
	{
		pD3dmDevice_->Release();
	}

    if(pD3DM_)
	{
        pD3DM_->Release();
	}

	CloseHandle(hCRT);
}


//! Return a D3DM Compositing surface
QSharedPointer<CompositingSurface> D3DMVisualTarget::generateCompositingSurface()
{
	QSharedPointer<D3DMCompositingSurface> d3dmCompositingSurface(new D3DMCompositingSurface(pD3dmDevice_));

	return d3dmCompositingSurface;

}

QString D3DMVisualTarget::getTypeName()
{
	return QString("Direct3DMobile");
}

/*!
 *	draw adds the contents of the compositing surface to the list of textured
 *	quads to be rendered.  Since the lists are emptied every time present is called,
 *	draw must be called on each compositing surface every frame
 */
void D3DMVisualTarget::draw(QPoint location, QSharedPointer<CompositingSurface> compositingSurface)
{
	if(compositingSurface->getTypeName() == "Direct3DMobile")
	{
		//retreive a pointer to the texture
		LPDIRECT3DMOBILETEXTURE tex = compositingSurface.staticCast<D3DMCompositingSurface>()->getTexture();

		//add texture pointer to the rendering list
		textureList_.push_back(tex);

		D3DMSURFACE_DESC texDesc;
		tex->GetLevelDesc(0, &texDesc);

		unsigned int imgHeight = compositingSurface.staticCast<D3DMCompositingSurface>()->getImageHeight();

		//figure out the position for the sprite, and add it to the list
		//NOTE: Picto uses the standard screen coodinates with (0,0) in the top left
		//corner.  However, the D3DM setup we are using has (0,0) in the bottom left corner, so
		//we need to find the "complement" of the y-coordinate.
		//The value stored in the positionList_ is the bottom left corner (inclusive)
		//of the sprite.
		QPointF quadPoint;
		quadPoint.setX((float)location.x());
		quadPoint.setY((float)(height_-location.y()-imgHeight));
		positionList_.push_back(quadPoint);
	}
	else
	{
		d3dFail("D3DMVisualTarget: draw called with a compositing surface not of type \"Direct3DMobile\"");
	}
}

/*!
 *	present renders all of the textured quads in the current rendering lists, and then
 *  clears out the lists.
 */
void D3DMVisualTarget::present()
{
	LPDIRECT3DMOBILETEXTURE tex;
	QPointF quadPoint;
	D3DMXMATRIX matScale, matTrans;
	D3DMSURFACE_DESC texDesc;

	HRESULT hr;
	
	//clear the back buffer
    pD3dmDevice_->Clear( 0, NULL, D3DMCLEAR_TARGET, 0, 0.0f, 0 );


	//begin the scene
	hr = pD3dmDevice_->BeginScene();
    if(hr)
		d3dFail("D3DMVisualTarget: BeginScene FAILED");

	//render each textured quad
	while(!textureList_.isEmpty())
	{
		tex = textureList_.takeFirst();
		quadPoint = positionList_.takeFirst();

		tex->GetLevelDesc(0, &texDesc);

		//The 0.5 offset on the translation matrix is done to ensure perfect texture mapping
		D3DMXMatrixScaling(&matScale, (float)texDesc.Width, (float)texDesc.Height, 0.0f);
		D3DMXMatrixTranslation(&matTrans, quadPoint.x()-0.5f, quadPoint.y()-0.5f, 0.0f);
		pD3dmDevice_->SetTransform( D3DMTS_WORLD, (D3DMMATRIX*)&(matScale*matTrans), D3DMFMT_D3DMVALUE_FLOAT );

        pD3dmDevice_->SetTexture( 0, tex );
		pD3dmDevice_->DrawPrimitive(D3DMPT_TRIANGLELIST,
											0,		//Start Vertex
											2);		//Primitive Count


	}


	//end the scene
	hr = pD3dmDevice_->EndScene();
    if(hr)
		d3dFail("D3DMVisualTarget: EndScene FAILED");

	//Don't call present until we are in vertical blanking
	BYTE outBuf;
	DWORD bytesReturned;
		DeviceIoControl(hCRT,IOCTRL_WAIT_VBLANK_START,NULL,0,&outBuf,1,&bytesReturned,NULL);

	static int test = 0;

	//present the back buffer
    hr = pD3dmDevice_->Present( NULL, NULL, NULL, NULL );
    if(hr)
		d3dFail("D3DMVisualTarget: Present FAILED");
}

void D3DMVisualTarget::paintEvent(QPaintEvent *)
{
	//!  I'm not entirely sure how to do a paint event here...
}


void D3DMVisualTarget::drawNonExperimentText(QFont font, QColor color, QRect rect, Qt::AlignmentFlag alignment, QString text)
{
	//!  implement drawNonExperimentText for D3DMVisualTarget
}


//outputs an error message if something goes wrong with Direct3DMobile
void D3DMVisualTarget::d3dFail(QString errorMsg)
{
	QString text = "If you are seeing this message, something has gone ";
	text += "horribly wrong.  Try restarting the PictoBox.\n\n";
	text += "Error: " + errorMsg;

	QMessageBox errorMsgBox;
	errorMsgBox.setText(text);
	errorMsgBox.exec();

	QCoreApplication::exit(1);
}



}; //namespace Picto
