#ifndef _D3DCOMPOSITINGSURFACE_H_
#define _D3DCOMPOSITINGSURFACE_H_

#include <d3d9.h>
#include <QImage>

#include "../../common/compositor/CompositingSurface.h"

namespace Picto {

/*!	\brief A CompositingSurface that converts a QImage to a LPDIRECT3DTEXTURE9 for Direct3D
 *
 *	\details The D3DCompositingSurface is located in the PictoDirector source tree (as opposed to the
 *	libPicto Tree) because it is platform dependant.  The compositing surface uses
 *	Direct3D Textures to store image data, and then pases them to the D3DVisualTarget.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class D3DCompositingSurface : public CompositingSurface
{
public:
	D3DCompositingSurface(IDirect3DDevice9 *_pD3dDevice);
	virtual ~D3DCompositingSurface();

	void convertImage(QImage image);
	QString getTypeName();

	LPDIRECT3DTEXTURE9 getTexture();

	//! imageHight is neccessary, since once the QImage is placed on a texture, we can't tell how big the original image was
	unsigned int getImageHeight() { return imageHeight_; };
private:
	void d3dFail(QString);

	LPDIRECT3DTEXTURE9 texture_;
	IDirect3DDevice9 *pD3dDevice_;

	unsigned int imageHeight_;
};


}; //namespace Picto

#endif
