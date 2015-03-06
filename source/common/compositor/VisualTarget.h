#ifndef _VISUALTARGET_H_
#define _VISUALTARGET_H_

#include <QColor>
#include <QSharedPointer>
#include <QWidget>
#include <QKeyEvent>

#include "../common.h"

#include "CompositingSurface.h"
#include "../timing/Timestamper.h" 
namespace Picto {


/*!	\brief The surface on which all graphics are rendered.
 *
 *	This is a base class for the visual rendering targets.  The idea of the visual rendering system is 
 *	that the VisualElements that are part of the experimental design create QImage objects of the graphics that
 *	they represent.  Whenever a State is active, it takes all of the QImage objects of VisualElements
 *	that are in scope and adds them to its Scene.  The scene creates compositing surfaces from the visual
 *	target that is being rendered to and uses one of these ComposingSurfaces to translate each QImage to
 *	the format required by the VisualTarget.  These compositing surfaces are then added to the VisualTarget
 *	and rendered once per frame.
 *	Some simple example code that removes all of the larger architectural issues and simply creates and 
 *	renders an image appears below:
 *		\code
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
 *		\endcode
 *	\details The VisualTarget is inherited by classes that render to particular platforms with particular features.
 *	The PixmapVisualTarget for example renders a simple CPU processed surface to the display.  The Director's 
 *	D3D Visual Target on the other hand renders using the graphics card at a much higher speed and with precisely
 *	trackable timing.  Each of these implements the VisualTarget for a different type of application, but the 
 *	Experimental presentation system needs only worry about the VisualTarget base class.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
	/*! \brief Creates and returns a new CompositingSurface that can be used to translate a QImage into a graphical data structure that this VisualTarget can understand.
	 *	\details The returned CompositingSurface should set up with a QImage by the calling function.
	 */
	virtual QSharedPointer<CompositingSurface> generateCompositingSurface() = 0;
	/*! \brief Returns the name of this type of CompositingSurface */
	virtual QString getTypeName() = 0;
	/*!	\brief Draws the compositing surface onto the visual target.  The compositingSurfaceOffset is 
	 *	the offset at which the compositing surface should be drawn from location to assure that the 
	 *	"zero" point of the compositing surface is at location.  For example, a cursor compositing 
	 *	surface might request to be drawn at 0,0, however the center of the cursor might be at position 
	 *	5,5 or 7,7 within the compositing surface depending on the size of the cursor.  The offset 
	 *	tells the system, move the compositing surface up and to the left by 5,5 or 7,7 so that the 
	 *	center of the cursor is at 0,0.
	 *	We cant simply pass location-compositingSurfaceOffset into this function because in zooming
	 *	these two components need to be treated differently (See the pixmap implementation of this function)
	 *	\sa PixmapVisualTarget.cpp
	 */
	virtual void draw(QPoint location, QPoint compositingSurfaceOffset, QSharedPointer<CompositingSurface> compositingSurface) = 0;
	/*! \brief Renders the latest frame to the display.
	 *	\note This function calls setFirstPhosphorTime() internally and may block execution for up to
	 *	one frame depending on this VisualTarget type's level of accuracy.
	 */
	virtual void present() = 0;
	/*! \brief Clears the rendering buffer and display.
	 *	\note This function calls setFirstPhosphorTime() internally and may block execution for up to
	 *	one frame depending on this VisualTarget type's level of accuracy.
	 *	\sa draw()
	 */
	virtual void clear() = 0;
	virtual void setZoom(float zoom);
	/*! \brief Returns the last time at which the first phosphor was rendered.
	*/
	double getLatestFirstPhosphor(){return lastFrameTime_;};
	/*! \brief Returns true if the latest frame was successfully rendered, false otherwise.
	*/
	virtual bool latestFrameSuccesfullyRendered() = 0;
	virtual QRect getDimensions();
	virtual QPoint viewportPointToTargetPoint(QPoint viewportPoint);
	virtual QPoint targetPointToViewportPoint(QPoint targetPoint);

	/*! \brief Draws the current frame onto the input widget
	 *	\details In the workstation, where precise first phosphor timing isn't important, we draw the VisualTarget frame onto
	 *	a workstation widget according to paintEvents from the event loop.  This would generally just happen in the paintEvent
	 *	function, but we use a parent widget (VisualTargetHost) that calls this paint function to paint the latest frame onto
	 *	itself whenever its paintEvent is called.  I believe that this VisualTargetHost class is unnecessary and pretty much 
	 *	arose due to an incomplete understanding of the QWidget system.  Probably, we can just do away with the VisualTargetHost
	 *	class and use this class directly wherever that class is used.  In practice however, what we have works and we don't have
	 *	time to make this change and test it.  If you are reading this and you have time, go for it...
	 *	\sa VisualTargetHost
	 */
	virtual void paint(QPaintDevice *widget) = 0;

signals:
	/*! \brief Emitted when the VisualTarget determines that the first phosphor has been drawn to the display.*/
	void presented(double frameTime);
	/*! \brief Emitted when the user presses escape.
	 *	\details This is useful when the VisualTarget is running in full screen mode and we want there to be 
	 *	some way to exit out.
	 */
	void escapePressed();

protected:
	/*! \brief Paints the latest frame onto this widget
	 * \sa paint
	 */
	void paintEvent(QPaintEvent *) { paint(this); };
	void keyPressEvent(QKeyEvent* event);
	void setFirstPhosphorTime();

	bool bWindowed_;
	int width_;
	int height_;
	float zoom_;
	Timestamper stamper_;
	double lastFrameTime_;

};

}; //namespace Picto

#endif
