#ifndef _D3DCOMPOSITINGSURFACE_H_
#define _D3DCOMPOSITINGSURFACE_H_

#include <d3d9.h>
#include <QImage>

#include "../../common/compositor/CompositingSurface.h"

namespace Picto {

class D3DCompositingSurface : public CompositingSurface
{
public:
	D3DCompositingSurface(IDirect3DDevice9 *_pD3dDevice);
	~D3DCompositingSurface();

	void convertImage(QImage image);
	QString getTypeName();

	LPDIRECT3DTEXTURE9 getTexture();

	//! imageHight is neccessary, since one the QImage is placed on a texture, we can't tell how big the original image was
	unsigned int getImageHeight() { return imageHeight_; };
private:
	void d3dFail(QString);

	LPDIRECT3DTEXTURE9 texture_;
	IDirect3DDevice9 *pD3dDevice_;

	unsigned int imageHeight_;
};


}; //namespace Picto

#endif
