/*! \file blankscreen/main.cpp
 *
 *	\brief A really simple embedded apllication that displays a blank screen
 *
 *	This is a simple test application designed to be run on PictoBox.  It was originally
 *	designed so that we could test the Windows CE Platform that we had built, but it is
 *	generally usable as a way to confirm that any device is capable of running Qt.
 */

#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QDesktopWidget>
#include <QPalette>
#include <QSharedPointer>
#include <QFile>
#include <QHBoxLayout>
#include <QPainter>

#ifdef WINCE
#include <winbase.h>
#endif
#include <qsqldatabase>
#include <qsqlquery>
#include "../../common/common.h"
#include "../../common/globals.h"
#include "../../common/namedefs.h"
//#include "../../common/archives/archives.h"
//#include "../../common/random/mtrand.h"
//#include "../../common/Storage/Asset.h"
//#include "../../common/Experiment/Experiment.h"
//#include "../../common/parameter/OperatorClickParameter.h"
//#include "../../common/storage/propertyfactory.h"
//#include "../../common/property/PropertyContainer.h"
//#include "../../common/controlelements/stopwatchController.h"
//#include "../../common/statemachine/State.h"
//#include "../../common/statemachine/StateMachine.h"
//#include "../../common/compositor/PixmapVisualTarget.h"
//#include "../../common/compositor/VisualTargetHost.h"
//#include "../../common/compositor/RenderingTarget.h"
//#include "../../common/compositor/PCMAuralTarget.h"
//#include "../../common/iodevices/AudioRewardController.h"
//#include "../../common/iodevices/NullEventCodeGenerator.h"
//#include "../../common/engine/MouseInputPort.h"
//#include "../../common/engine/XYSignalChannel.h"
#include "../../common/mainmemleakdetect.h"

using namespace Picto;


class BlankWindow : public QWidget
{
	//QSharedPointer<Asset> test;
public:
	void BlankWindow::image2Pixmap(QImage &img,QPixmap &pixmap)
	{
	   // Convert the QImage to a QPixmap for display
	   pixmap = QPixmap(img.size());
	   QPainter painter;
	   painter.begin(&pixmap);
	   painter.drawImage(0,0,img);
	   painter.end();
	}

    BlankWindow(QWidget *parent = 0) : QWidget(parent)
    {
		QSqlDatabase newSession = QSqlDatabase::addDatabase("QSQLITE","TestConnection");
		newSession.setDatabaseName("C:\\Users\\joeys\\Desktop\\Temp\\Session_2013_02_20__10_27_19.sqlite");
		newSession.open();
		QSqlQuery query(newSession);
		query.prepare("SELECT dataid,timestamp,channel,unit,waveform FROM spikes WHERE dataid > 607 AND timestamp <= 325 ORDER BY dataid LIMIT 10000");
		query.exec();
		QString result;
		while(query.next())
		{
			result.append(QString("%1 %2 %3 %4 %5\n").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString()).arg(query.value(4).toString()).arg(query.value(5).toString()));
		}
		qDebug(result.toLatin1());
	}
};

int main(int argc, char *argv[])
{
	//This will cause memory leaks to print out on exit if they're enabled
	#if defined (DETECTMEMLEAKS) && defined(_MSC_VER) && defined(_DEBUG)
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif
	QApplication::setDesktopSettingsAware(false);
	QApplication a(argc, argv);

	//QString* memLeakStr = new QString();
  BlankWindow blankWindow;
  blankWindow.show();

  if(argc>1)
  {
#ifdef WINCE
     SignalStarted(atol(argv[1]));
#endif
  }

	return a.exec();
}
