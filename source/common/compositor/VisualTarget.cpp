#include "VisualTarget.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new VisualTarget with size defined by _width and _height and _bWindowed defining if rendering is windowed or full screeen.
 * \details _width and _height are defined in pixels, where maximum _width is 800 and miximum height is 600.  In Picto, everything is in terms
 *	of 800 X 600 resolution.  This is something that needs to be fixed at some point in a clever way such that old experiments can be automatically
 *	upgraded.
 */
VisualTarget::VisualTarget(bool _bWindowed, int _width, int _height) :
	bWindowed_(_bWindowed),
	width_(_width),
	height_(_height),
	lastFrameTime_(-1.0),
	zoom_(1.0)
{
	int i=0;
	i++;
}

VisualTarget::~VisualTarget()
{
	printf("Visual target destructor");
}

/*! \brief Sets the zoom value to be used when rendering scalable CompositingSurface objects.
 * \detail The zoom value must be greater than zero and less than 1, essentially, this value will
 * multiply all of the dimensions of the input CompositingSurface objects to create smaller
 * CompositingSurface object for a 'Zoom Out' effect.
 */
void VisualTarget::setZoom(float zoom)
{
	Q_ASSERT(zoom > 0);
	zoom_ = zoom;
}

/*! \brief Returns a QRect with the dimensions of this VisualTarget.
 */
QRect VisualTarget::getDimensions()
{
	return(QRect(0,0,width_,height_));
}

/*! \brief Converts a point on the viewport to a point in the VisualTarget itself.
 * \details For example, if the viewport has zoomed out, a point on the top left of
 * the viewport will no longer be at 0,0 within the VisualTarget but rather at somewhere
 * below zero like -100,-100
 */
QPoint VisualTarget::viewportPointToTargetPoint(QPoint viewportPoint)
{
	return viewportPoint;
}

/*! \brief Converts a point in the VisualTarget to a point on the Viewport window.
 * \details For example, if the viewport has zoomed out, a point at 10,10 in the 
 * VisualTarget might only be at 1,1 in the viewport.
 */
QPoint VisualTarget::targetPointToViewportPoint(QPoint targetPoint)
{
	return targetPoint;
}

/*! \brief Overrides the QWidget function so that we can emit an escapePressed signal whenever that
 * key is pressed.
 * \details This is useful for cases when the VisualTarget is rendering in full screen and we need 
 * a way to detect that the user wants to leave the full screen environment.
 */
void VisualTarget::keyPressEvent(QKeyEvent* event)
{
	QWidget::keyPressEvent(event);
	if(event->key() == Qt::Key_Escape)
		emit escapePressed();
}

/*! \brief Stamps the current time as the latest first phosphor time and emits a presented() signal
 * with that time.
 * \details The first phosphor time is the time at which the first phosphor appears on the top left
 * corner of the user's display.  It is important to track this timing so that we can precisely 
 * relate images that the user is monitoring on the screen to neural activity.  In fact, in Picto analyses
 * all behavioral data is timed according to first phosphor times since these are the times at which actions
 * affect the user and no other times have any real physical meaning.
 * The rendering code in inheriting classes needs to make sure to call this function as soon as they
 * detect that the first phosphor was rendered to the screen.  Depending on which VisualTarget is used
 * this will sometimes be more or less accurate of a representation of the true first phosphor time.
 */
void VisualTarget::setFirstPhosphorTime()
{
	lastFrameTime_ = stamper_.stampSec();
	
	//If this VisualTarget belongs to a VisualTargetHost, we will know that a repaint occured.
	//Also, others can use this as a signal that the firstPhosphor just occured.
	emit presented(lastFrameTime_);
}

}; //namespace Picto
