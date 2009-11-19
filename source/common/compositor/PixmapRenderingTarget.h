#ifndef _PIXMAPRENDERINGTARGET_H_
#define _PIXMAPRENDERINGTARGET_H_

#include "../common.h"

#include "VisualTarget.h"
#include "AuralTarget.h"

#include <QWidget>
#include <QHostAddress>
#include <QSharedPointer>

namespace Picto {

#if defined WIN32 || defined WINCE
struct PICTOLIB_API PixmapRenderingTarget : QWidget
#else
struct PixmapRenderingTarget : QWidget
#endif
{
	Q_OBJECT

public:
    PixmapRenderingTarget(QSharedPointer<VisualTarget> visualTarget, QSharedPointer<AuralTarget> auralTarget, bool bWindowed = false, int width = 800, int height = 600, QWidget *parent = 0);

	QSharedPointer<CompositingSurface> generateCompositingSurface();
	QSharedPointer<MixingSample> generateMixingSample();

	void updateStatus(QString status);
	void showSplash(bool visible=true);

protected:
	void paintEvent(QPaintEvent *);

private:
	QSharedPointer<VisualTarget> visualTarget_;
	QSharedPointer<AuralTarget> auralTarget_;

	bool bWindowed_;
	bool bShowSplash_;
	QString status_;

private slots:
	void foundServer(QHostAddress serverAddress, quint16 serverPort);
	void discoverServerFailed();
};

#endif

}; //namespace Picto
