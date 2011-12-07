
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

void VisualTargetHost::setVisualTarget(QSharedPointer<VisualTarget> target)
{
	if(target.isNull())
		return;
	target_ = target;
	QRect dims = target->getDimensions();

	setFixedSize(dims.width(),dims.height());
	resize(dims.width(),dims.height());

	connect(target_.data(), SIGNAL(presented()), this, SLOT(update()));
}

void VisualTargetHost::paintEvent(QPaintEvent*)
{
	if(!target_.isNull())
		target_->paint(this);
}

void VisualTargetHost::mousePressEvent(QMouseEvent *event)
{
	emit clickDetected(event->pos());
	QWidget::mousePressEvent (event);
}

}; //namespace Picto
