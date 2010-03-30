/*!	\brief a compositing surface for DirectDraw (used on PictoBox)
 *	
 *	The DirectDraw compositing surface is used on PictoBox (which is running
 *	WindowsCE.  DirectDraw is used to provide the maximum amount of speed
 *	and hardware control.  Obviously, this code should only be included
 *	if we're building for the embedded system. 
 *
 *	WARNING: Running in WindowsCE on the PictoBox, we found that any transparency
 *	(color-keyed or alpha) resulted in extremely slow rendering times.  As a result
 *	this code was abandoned and should be considered to be in an unknown state
 */

#ifndef _DDRAWCOMPOSITINGSURFACE_H_
#define _DDRAWCOMPOSITINGSURFACE_H_

#include <ddraw.h>

#include "common.h"
#include "CompositingSurface.h"

namespace Picto {

class DDrawCompositingSurface : public CompositingSurface
{
public:
	DDrawCompositingSurface(LPDIRECTDRAW dDraw_);
	~DDrawCompositingSurface();

	void convertImage(QImage image);
	QString getTypeName();

	LPDIRECTDRAWSURFACE getSurface();

private:
	LPDIRECTDRAWSURFACE surface_;
	LPDIRECTDRAW dDraw_;

};


}; //namespace Picto

#endif
