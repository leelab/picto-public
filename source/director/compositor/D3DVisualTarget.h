#ifndef _D3DVISUALTARGET_H_
#define _D3DVISUALTARGET_H_

#include "../../common/compositor/VisualTarget.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <D3dx9core.h>
#include <QList>

namespace Picto {

/*!	\brief An implementation of VisualTarget within the Direct3D API.  For use on any windows system.
 *	
 *	\details This VisualTarget is clearly limited to Windows platforms.  Additionally, the
 *	platform must support DirectX9.  We are using Direct3D for a number of reasons:
 *	- It gives us full control over the frame rate
 *	- DirectDraw isn't really supported (outside of WinCE and Win7)
 *	- Video card drivers always support Direct3D
 *	
 *	In general, Direct3D is very useful in that it is very fast and very good at providing precise vSync timing 
 *	for the purpose of marking first phosphor times.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class D3DVisualTarget : public VisualTarget
{
public:
	D3DVisualTarget(bool timingCritical, QColor bgColor);
	virtual ~D3DVisualTarget();

	QSharedPointer<CompositingSurface> generateCompositingSurface();
	QString getTypeName();
	void draw(QPoint location, QPoint compositingSurfaceOffset, QSharedPointer<CompositingSurface> compositingSurface);
	void present();
	void clear();
	/*! \brief Returns true if the latest frame was succesfully rendered (ie. The present() function didn't fail somewhere in the middle).*/
	bool latestFrameSuccesfullyRendered(){return renderSuccess_;};

	void drawNonExperimentText(QFont font, QColor color, QRect rect, Qt::AlignmentFlag alignment, QString text);

protected:
	void paint(QPaintDevice *widget);
	void closeEvent(QCloseEvent *event);

private:
	void initializeD3DDevice();
	void d3dFail(QString);

	IDirect3D9*        pD3D_;  // Used to create the D3DDevice
	IDirect3DDevice9*  pD3dDevice_;  // Our rendering device
	HANDLE hCRT;
	HWND hWnd_;
	D3DPRESENT_PARAMETERS d3dpp_; 



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
	bool renderSuccess_;


	QColor BgCol_;
};

}; //namespace Picto

#endif