/*!	\brief A compositing surface for Direct3D Mobile (used on PictoBox)
 *	
 *	The Direct3D Mobile compositing surface is used on PictoBox (which is running
 *	WindowsCE.  Direct3D Mobile is used to provide the maximum amount of speed
 *	and hardware control.  Obviously, this code should only be included
 *	if we're building for the embedded system. 
 */

#ifndef _D3DMCOMPOSITINGSURFACE_H_
#define _D3DMCOMPOSITINGSURFACE_H_

#include <d3dm.h>

#include "common.h"
#include "CompositingSurface.h"

namespace Picto {

class D3DMCompositingSurface : public CompositingSurface
{
public:
	D3DMCompositingSurface(LPDIRECT3DMOBILEDEVICE _pD3dmDevice);
	~D3DMCompositingSurface();

	void convertImage(QImage image);
	QString getTypeName();

	LPDIRECT3DMOBILETEXTURE getTexture();

	//! imageHight is neccessary, since one the QImage is placed on a texture, we can't tell how big the original image was
	unsigned int getImageHeight() { return imageHeight_; };

private:
	void d3dFail(QString);

	LPDIRECT3DMOBILETEXTURE texture_;
	LPDIRECT3DMOBILEDEVICE  pD3dmDevice_;

	unsigned int imageHeight_;

};


}; //namespace Picto

#endif
