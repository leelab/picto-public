
#include <QRect>

#include "VisualTargetHost.h"
#include "../memleakdetect.h"
namespace Picto {

VisualTargetHost::VisualTargetHost()
{
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);

	setAutoFillBackground(true);
}
/*! \brief Sets the VisualTarget object that will be hosted by this VisualTargetHost.
 */
void VisualTargetHost::setVisualTarget(QSharedPointer<VisualTarget> target)
{
	if(target.isNull())
		return;
	target_ = target;
	QRect dims = target->getDimensions();

	//setMaximumSize(dims.width(),dims.height());
	//setFixedSize(dims.width(), dims.height());
	resize(dims.width(),dims.height());

	connect(target_.data(), SIGNAL(presented(double)), this, SLOT(presented(double)));
}

/*! \brief Paints the underlying VisualTarget objects frame onto this object
 *	\details This is called by the QWidget hierearchy whenever this widget needs
 *	to be repainted.  Since this objects whole Raison d'etre is to work around
 *	the QWidget system, we need to fake the QWidget parent relationship by 
 *	manually drawing the VisualTarget "child" object's frame onto this object
 */
void VisualTargetHost::paintEvent(QPaintEvent*)
{
	if(!target_.isNull())
		target_->paint(this);
}

/*! \brief Emits a clickDetected signal whenever someone clicks on this window.
 */
void VisualTargetHost::mousePressEvent(QMouseEvent *event)
{
	if(target_.isNull())
		return;
	emit clickDetected(target_->viewportPointToTargetPoint(event->pos()));
	QWidget::mousePressEvent (event);
}

void VisualTargetHost::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);
	if (!target_.isNull())
	{
		target_->resizeEvent(event);
		update();
	}
}

/*! \brief Causes this VisualTargetHost to be repainted.
 *	\details Whenever the underlying VisualTarget is repainted, this object
 *	needs to be repainted too.  With the regular QWidget hierarchy, this would happen
 *	automatically, but here we are faking that hierarchy so we do it manually.
 *	\sa paintEvent
 */
void VisualTargetHost::presented(double)
{
	update();
}

const QPoint VisualTargetHost::viewportPointToTargetPoint(const QPoint point) const
{
	if (!target_.isNull())
	{
		return target_->viewportPointToTargetPoint(point);
	}

	return point;
}


}; //namespace Picto
