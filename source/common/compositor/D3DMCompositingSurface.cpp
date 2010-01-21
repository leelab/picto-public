#include <QImage>
#include <QApplication>
#include <QMessageBox>

#include "D3DMCompositingSurface.h"

namespace Picto {

D3DMCompositingSurface::D3DMCompositingSurface(LPDIRECT3DMOBILEDEVICE _pD3dmDevice)
{
	pD3dmDevice_ = _pD3dmDevice;
	texture_ = 0;
}

D3DMCompositingSurface::~D3DMCompositingSurface()
{
	if(texture_)
	{
		texture_->Release();
		texture_ = 0;
	}
	//Note that the D3DM device does not belong to us (it belongs to the 
	//Visual Target), so it is not our responsibility to release it.
}

/*!	convertImage takes the passed in QImage and converts it to a Direct3D
 *	Mobile texture, which is then stored as a private class variable.
 */
void D3DMCompositingSurface::convertImage(QImage image)
{
	//If we already have a valid texture, we need to clean it up and 
	//start over.
	if(texture_)
	{
		texture_->Release();
		texture_ = 0;
	}
	
    // Determine if the device can create textures in video memory
    // by testing the device caps bits.
	D3DMCAPS caps;
	HRESULT hr;
    hr = pD3dmDevice_->GetDeviceCaps(&caps);
	if(hr)
		d3dFail("D3DMCompositingSurface: GetDeviceCaps FAILED");


    D3DMPOOL texpool;
    if (caps.SurfaceCaps & D3DMSURFCAPS_VIDTEXTURE)
        texpool = D3DMPOOL_VIDEOMEM;
    else
        texpool = D3DMPOOL_SYSTEMMEM;

	//Figure out the dimensions of the texture.
	//NOTE: The following comments apply only to the D3DM drivers supplied by VIA
	//for the EPIA-M700 motherboard that is installed in PictoBox as of Jan 20, 2010.
	//If you look at the device caps it seems to suggest that any size texture will work
	//However, experimentally I have found that both dimensions must be powers of two.
	//(however rectangular textures will work).
	int texWidth = 1;
	int texHeight = 1;

	while(texWidth < image.width())
		texWidth *= 2;
	while(texHeight < image.height())
		texHeight *= 2;

	//store the image height (since this information will be lost when we
	//place the image on a texture).  We need this info to accurately determine
	//the coordinates
	imageHeight_ = image.height();

	//Create a texture with dimensions large enough for the image
	hr = pD3dmDevice_->CreateTexture(texWidth,texHeight,1,D3DMUSAGE_LOCKABLE,D3DMFMT_A8R8G8B8,texpool,&texture_);
	if(hr)
		d3dFail("D3DMCompositingSurface: CreateTexture FAILED");

	//if needed, convert the image format
	if(image.format() != QImage::Format_ARGB32)
		image = image.convertToFormat(QImage::Format_ARGB32);

	
	//lock the entire texture
	D3DMLOCKED_RECT lockedRect;
	hr = texture_->LockRect(0,&lockedRect,0,0);
	if(hr)
		d3dFail("D3DMCompositingSurface: texture_->LockRect FAILED");

	int x,y;
	DWORD *pTexel;
	pTexel = (DWORD*)lockedRect.pBits;

	//clear the existing texture
	memset(pTexel,0,sizeof(DWORD)*texHeight*lockedRect.Pitch);

	//NOTE: The following code assumes that the pitch is a multiple of 4
	//so far this has always been true.  If there are rendering problems, 
	//this may be a cause...
	Q_ASSERT_X(lockedRect.Pitch%4 == 0, "D3DMCompositingSurface::convertImage", "pitch not a multiple of 4");

	//NOTE: QImage has the origin in the top-right corner, while D3DM has it in the 
	//bottom left, so coordinate conversion is neccessary.
	for(y=0;y<image.height(); y++)
	{
		for(x=0; x<image.width(); x++)
		{
			DWORD index = x+y*lockedRect.Pitch/4;
			*(pTexel+index) = image.pixel(x,image.height()-y-1);
		}
	}

	texture_->UnlockRect(0);
}

QString D3DMCompositingSurface::getTypeName()
{
	return QString("Direct3DMobile");
}

LPDIRECT3DMOBILETEXTURE D3DMCompositingSurface::getTexture()
{
	return texture_;
}

//outputs an error message if something goes wrong with Direct3DMobile
void D3DMCompositingSurface::d3dFail(QString errorMsg)
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
