/*!	\brief A visual target that uses the Direct3D interface.  For use on any windows system.
 *	
 *
 *	The functions drawNonExperimentText and paintEvent are not currently supported
 */

#ifndef _D3DVISUALTARGET_H_
#define _D3DVISUALTARGET_H_

//#include "../common.h"

#include "../../common/compositor/VisualTarget.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <D3dx9core.h>
#include <QList>

namespace Picto {

//#if defined WIN32 || defined WINCE
//class PICTOLIB_API D3DVisualTarget : public VisualTarget
//#else
class D3DVisualTarget : public VisualTarget
//#endif
{
public:
	D3DVisualTarget();
	~D3DVisualTarget();

	QSharedPointer<CompositingSurface> generateCompositingSurface();
	QString getTypeName();
	void draw(QPoint location, QSharedPointer<CompositingSurface> compositingSurface);
	void present();

	void drawNonExperimentText(QFont font, QColor color, QRect rect, Qt::AlignmentFlag alignment, QString text);

protected:
	void paintEvent(QPaintEvent *);

private:
	void d3dFail(QString);

	IDirect3D9*        pD3D_;  // Used to create the D3DDevice
	IDirect3DDevice9*  pD3dDevice_;  // Our rendering device
	HANDLE hCRT;



	//Vertex and index buffers and buffer data.  
	//Note that since we are using textured quads, the buffers contain noting more than 
	//a 1x1 quad.  This quad is scaled and translated to draw a sprite.
	LPDIRECT3DVERTEXBUFFER9 pVertexBuffer_; // Buffer to hold vertices

	struct CUSTOMVERTEX
	{
		float x, y, z;
		D3DCOLOR color;
		float u,v;		//texture coords
	};

	//texture and position lists
	//Note that these lists are aligned so that textureList[x]
	//has position = positionList[x]
	//Also, the location list uses D3D coordinates, which means that the origin
	//is in the bottom left corner, and the units are pixels
	QList<LPDIRECT3DTEXTURE9> textureList_;
	QList<QPointF> positionList_;


};

}; //namespace Picto

#endif