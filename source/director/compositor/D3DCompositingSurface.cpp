#include <QMessageBox>
#include <QApplication>


#include "D3DCompositingSurface.h"

namespace Picto {


D3DCompositingSurface::D3DCompositingSurface(IDirect3DDevice9 *_pD3dDevice)
{
	pD3dDevice_ = _pD3dDevice;
	texture_ = 0;
}

D3DCompositingSurface::~D3DCompositingSurface()
{
	if(texture_)
	{
		texture_->Release();
		texture_ = 0;
	}
	//Note that the D3DM device does not belong to us (it belongs to the 
	//Visual Target), so it is not our responsibility to release it.
}

/*!	\brief Converts a QImage to a D3D Texture.
 *
 *	We use DIRECT3DTEXTURE9 objects to store the image data.  What convertImage
 *	is called, the passed in QImage gets written to a texture of the
 *	appropriate size.  This texture is then eventually returned to the 
 *	D3DVisualTarget which draws it on a quad.
 */
void D3DCompositingSurface::convertImage(QImage image)
{	
	HRESULT hr;

	//Figure out the dimensions of the texture.
	//NOTE: This assumes that the textures must be 2^n x 2^m
	int texWidth = 1;
	int texHeight = 1;

	while(texWidth < image.width())
		texWidth *= 2;
	while(texHeight < image.height())
		texHeight *= 2;

	//If we don't have a valid texture, or is the incorrect size, 
	//we will need to generate a new texture interface
	bool generateNewTexture = false;
	if(!texture_)
	{
		generateNewTexture = true;
	}
	else
	{
		 D3DSURFACE_DESC texDesc;
		 texture_->GetLevelDesc(0,&texDesc);
		 if(texDesc.Height != texHeight)
			 generateNewTexture = true;
		 else if(texDesc.Width != texWidth)
			 generateNewTexture = true;
	}

	if(generateNewTexture)
	{
		if(texture_)
			texture_->Release();

		// Determine if the device can create textures in video memory
		// by testing the device caps bits.
		D3DCAPS9 caps;
		hr = pD3dDevice_->GetDeviceCaps(&caps);
		if(hr)
			d3dFail("D3DCompositingSurface: GetDeviceCaps FAILED");

		//Create a texture with dimensions large enough for the image
		hr = pD3dDevice_->CreateTexture(texWidth,texHeight,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&texture_, NULL);
		if(hr)
			d3dFail("D3DCompositingSurface: CreateTexture FAILED");
	}

	//store the image height (since this information will be lost when we
	//place the image on a texture).  We need this info to accurately determine
	//the coordinates
	imageHeight_ = image.height();

	//if needed, convert the image format
	if(image.format() != QImage::Format_ARGB32)
		image = image.convertToFormat(QImage::Format_ARGB32);

	
	//lock the entire texture
	D3DLOCKED_RECT lockedRect;
	hr = texture_->LockRect(0,&lockedRect,0,0);
	if(hr)
		d3dFail("D3DCompositingSurface: texture_->LockRect FAILED");

	DWORD *pTexel;
	pTexel = (DWORD*)lockedRect.pBits;

	//clear the existing texture
	memset(pTexel,0,texHeight*lockedRect.Pitch);


	//NOTE: The following code assumes that the pitch is a multiple of 4
	//so far this has always been true.  If there are rendering problems, 
	//this may be a cause...
	Q_ASSERT_X(lockedRect.Pitch%4 == 0, "D3DCompositingSurface::convertImage", "pitch not a multiple of 4");

	//be sure to use a const pointer since we won't be modifying the image data and want to avoid an expensive and unnecessary deep copy
	const uchar * imageData = image.bits();
	const unsigned int * imageDataUint32 = (const unsigned int *) imageData;

	unsigned int upperBound = (unsigned int) pTexel + image.height() * lockedRect.Pitch;
	unsigned int incrementAmount = lockedRect.Pitch;
	unsigned int widthBound = image.width();

	unsigned int rowDataPointer, columnDataOffset;
	
	for(rowDataPointer=(unsigned int) pTexel;rowDataPointer<upperBound; rowDataPointer+=incrementAmount)
	{
		for(columnDataOffset=0; columnDataOffset < widthBound; columnDataOffset++)
		{
			*((unsigned int *) (rowDataPointer+columnDataOffset*sizeof(int))) = *imageDataUint32++;
		}

	}
	texture_->UnlockRect(0);

}

QString D3DCompositingSurface::getTypeName()
{
	return "Direct3D";
}

LPDIRECT3DTEXTURE9 D3DCompositingSurface::getTexture()
{
	return texture_;
}

//! outputs an error message if something goes wrong with Direct3DMobile
void D3DCompositingSurface::d3dFail(QString errorMsg)
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
