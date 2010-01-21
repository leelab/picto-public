/*!	\brief A visual target that uses the Direct3D Mobile interface.  For use on PictoBox.
 *	
 *	This visual target uses the Direct3D Mobile interface to present visual stimuli.
 *	Direct3D Mobile is only supported by Windows CE, so this code is only included
 *	if the target OS is WinCE.  
 *
 *	The functions drawNonExperimentText and paintEvent are not currently supported
 */

#ifndef _D3DMVISUALTARGET_H_
#define _D3DMVISUALTARGET_H_

#include "../common.h"

#include "VisualTarget.h"

#include <d3dm.h>
#include <d3dmx.h>
#include <QWidget>
#include <QList>

namespace Picto {

struct PICTOLIB_CLASS D3DMVisualTarget : public VisualTarget, public QWidget
{
public:
	D3DMVisualTarget();
	~D3DMVisualTarget();

	QSharedPointer<CompositingSurface> generateCompositingSurface();
	QString getTypeName();
	void draw(QPoint location, QSharedPointer<CompositingSurface> compositingSurface);
	void present();

	void drawNonExperimentText(QFont font, QColor color, QRect rect, Qt::AlignmentFlag alignment, QString text);

protected:
	void paintEvent(QPaintEvent *);

private:
	void d3dFail(QString);

	LPDIRECT3DMOBILE        pD3DM_;  // Used to create the D3DMDevice
	LPDIRECT3DMOBILEDEVICE  pD3dmDevice_;  // Our rendering device


	//Vertex and index buffers and buffer data.  
	//Note that since we are using textured quads, the buffers contain noting more than 
	//a 1x1 quad.  This quad is scaled and translated to draw a sprite.
	LPDIRECT3DMOBILEVERTEXBUFFER pVertexBuffer_; // Buffer to hold vertices
	LPDIRECT3DMOBILEINDEXBUFFER pIndexBuffer_; // Buffer to hold indices

	struct CUSTOMVERTEX
	{
		float x, y, z;
		D3DMCOLOR color;
		float u,v;		//texture coords
	};

	//texture and position lists
	//Note that these lists are aligned so that textureList[x]
	//has position = positionList[x]
	//Also, the location list uses D3DM coordinates, which means that the origin
	//is in the bottom left corner, and the units are pixels
	QList<LPDIRECT3DMOBILETEXTURE> textureList_;
	QList<QPointF> positionList_;


};

}; //namespace Picto

#endif
