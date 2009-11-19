#include "PixmapRenderingTarget.h"

#include <QApplication>
#include <QPainter>
#include <QDesktopWidget>

namespace Picto {

PixmapRenderingTarget::PixmapRenderingTarget(QSharedPointer<VisualTarget> visualTarget,
											 QSharedPointer<AuralTarget> auralTarget, 
											 bool bWindowed, 
											 int width, 
											 int height, 
											 QWidget *parent) :
	bWindowed_(bWindowed),
	bShowSplash_(false),
	QWidget(parent)
{
	visualTarget_ = visualTarget;
	auralTarget_ = auralTarget;

	if(!bWindowed)
	{
		QWidget::setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

		QRect screenRect = QApplication::desktop()->screenGeometry(-1);
		resize(screenRect.width(),screenRect.height());
		move(0,0);
	}
	else
	{
		resize(width, height);
	}

    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);
}

QSharedPointer<CompositingSurface> PixmapRenderingTarget::generateCompositingSurface()
{
	return visualTarget_->generateCompositingSurface();
}

QSharedPointer<MixingSample> PixmapRenderingTarget::generateMixingSample()
{
	return auralTarget_->generateMixingSample();
}

void PixmapRenderingTarget::updateStatus(QString status)
{
	status_ = status;
	update();
}

void PixmapRenderingTarget::showSplash(bool visible)
{
	bShowSplash_ = visible;
}

void PixmapRenderingTarget::paintEvent(QPaintEvent *)
{
	QPainter painter(this);

	if(bShowSplash_)
	{
		QImage image(":/common/images/splash.png");
		painter.drawImage((width()-image.width())/2,(height()-image.height())/2,image);

		painter.setPen(Qt::white);
		painter.setFont(QFont("Arial",18));
		painter.drawText(QRect(0,
							   (height()-image.height())/2+image.height()+25,
							   width(),
							   50),
						 Qt::AlignCenter,
						 status_);
	}
}

void PixmapRenderingTarget::foundServer(QHostAddress serverAddress, quint16 serverPort)
{
	updateStatus(QString("Found server at %1:%2").arg(serverAddress.toString()).arg(serverPort));
}

void PixmapRenderingTarget::discoverServerFailed()
{
	updateStatus("Server discover failed");
}

}; //namespace Picto
