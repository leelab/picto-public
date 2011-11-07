#ifndef _VISUALTARGET_H_
#define _VISUALTARGET_H_

#include <QColor>
#include "../common.h"

#include "CompositingSurface.h"
#include "../timing/Timestamper.h" 

#include <QSharedPointer>
#include <QWidget>

namespace Picto {

/*!	\brief Handles the visual rendering
 *
 *	This is a base class for the visual rendering targets.  The best way to explain
 *	these is to show an example of one:
 *
 *		QSharedPointer<Picto::D3DMVisualTarget> d3dmVisualTarget(new Picto::D3DMVisualTarget());
 *		d3dmVisualTarget->show();
 *
 *		QSharedPointer<Picto::CompositingSurface> compositingSurface;
 * 		compositingSurface = renderingTarget.generateCompositingSurface();
 *
 *		QSharedPointer<Picto::CircleGraphic> circleGraphic(new Picto::CircleGraphic(QPoint(50,50),100,QColor(0,255,0,127)));
 *		circleGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);\
 *
 *		// This is the actual rendering loop
 *		while(true)
 *		{
 *			d3dmVisualTarget->draw(circleGraphic->getPosition(), circleGraphic->getCompositingSurface("Direct3DMobile"));
 *			d3dmVisualTarget->present();
 *		}
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API VisualTarget :  public QWidget
#else
class VisualTarget : public QWidget
#endif
{
	Q_OBJECT
	friend class VisualTargetHost;
public:
	VisualTarget(bool _bWindowed = false, int _width = 800, int _height = 600);
	virtual ~VisualTarget();

	virtual QSharedPointer<CompositingSurface> generateCompositingSurface() = 0;
	virtual QString getTypeName() = 0;
	virtual void draw(QPoint location, QSharedPointer<CompositingSurface> compositingSurface) = 0;
	virtual void present() = 0;
	virtual void clear() = 0;
	double getLatestFirstPhosphor(){return lastFrameTime_;};
	virtual QRect getDimensions();

	//! \todo Add a function that returns time to VSynch
signals:
	void presented();

protected:
	//The seperate paint and paintEven functions allow us to paint this widget's
	//contents onto a different widget.  This is needed for the VisualTargetHost.
	virtual void paint(QPaintDevice *widget) = 0;
	void paintEvent(QPaintEvent *) { paint(this); };
	void setFirstPhosphorTime(){lastFrameTime_ = stamper_.stampSec();};

	bool bWindowed_;
	int width_;
	int height_;
	Timestamper stamper_;
	double lastFrameTime_;

};


}; //namespace Picto

#endif
