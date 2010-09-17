#ifndef _D3DCOMPOSITINGSURFACE_H_
#define _D3DCOMPOSITINGSURFACE_H_

#include <d3d9.h>
#include <QImage>

#include "../../common/compositor/CompositingSurface.h"

namespace Picto {

/*!	\brief A CompositingSurface that uses Direct3D
 *
 *	The D3D Compositing surface is located in the PictoDirector tree (as opposed to the
 *	libPictoTree) because it is platform dependant.  The compositing surface uses
 *	Direct3D Textures to store image data, and then pases them to the Visual Target.
 */
class D3DCompositingSurface : public CompositingSurface
{
public:
	D3DCompositingSurface(IDirect3DDevice9 *_pD3dDevice);
	~D3DCompositingSurface();

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
