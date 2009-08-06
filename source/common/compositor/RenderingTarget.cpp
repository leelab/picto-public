/*! \file RenderingTarget.cpp
 * \ingroup picto_compositor
 * \brief
 */

#include "RenderingTarget.h"

#include <QApplication>
#include <QPainter>
#include <QDesktopWidget>

namespace Picto {

RenderingTarget::RenderingTarget(bool bWindowed, QWidget *parent) :
	bWindowed_(bWindowed),
	bShowSplash_(false),
	QWidget(parent)
{
	if(!bWindowed)
	{
		QWidget::setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

		QRect screenRect = QApplication::desktop()->screenGeometry(-1);
		resize(screenRect.width(),screenRect.height());
		move(0,0);
	}
	else
	{
		resize(800, 600);
	}

    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);
}

void RenderingTarget::updateStatus(QString status)
{
	status_ = status;
	update();
}

void RenderingTarget::showSplash(bool visible)
{
	bShowSplash_ = visible;
}

void RenderingTarget::paintEvent(QPaintEvent *)
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

void RenderingTarget::foundServer(QHostAddress serverAddress, quint16 serverPort)
{
	updateStatus(QString("Found server at %1:%2").arg(serverAddress.toString()).arg(serverPort));
}

void RenderingTarget::discoverServerFailed()
{
	updateStatus("Server discover failed");
}

}; //namespace Picto
