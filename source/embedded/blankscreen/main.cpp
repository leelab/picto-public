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
#include <QtPropertyBrowser.h>
#include <QtVariantProperty.h>
#include <QtGroupBoxPropertyBrowser.h>
#include <qtbuttonPropertyBrowser.h>
#include <QPainter>
//#include <QtVariantProperty.h>

#ifdef WINCE
#include <winbase.h>
#endif
#include "QVideoEncoder.h"
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
  //      propManager1_ = QSharedPointer<QtVariantPropertyManager>(new QtVariantPropertyManager());
		//propManager2_ = QSharedPointer<QtVariantPropertyManager>(new QtVariantPropertyManager());
		//
		//QtVariantProperty *item1 = propManager1_->addProperty(QVariant::Int,"Number1");
		//QtVariantProperty *item2 = propManager1_->addProperty(QVariant::String,"Test String1");

		//QtVariantProperty *item3 = propManager2_->addProperty(QVariant::Int,"Number2");
		//QtVariantProperty *item4 = propManager2_->addProperty(QVariant::String,"Test String2");
		//propManager2_->clear();

		//propertyFactory_ = QSharedPointer<QtVariantEditorFactory>(new QtVariantEditorFactory(this));
		//
		//QtButtonPropertyBrowser* browser = new QtButtonPropertyBrowser();
		//browser->setFactoryForManager(propManager1_.data(), propertyFactory_.data());
		//browser->setFactoryForManager(propManager2_.data(), propertyFactory_.data());
		//foreach(QtProperty* prop,propManager1_->properties())
		//{
		//	browser->addProperty(prop);
		//}
		//foreach(QtProperty* prop,propManager2_->properties())
		//{
		//	browser->addProperty(prop);
		//}
		//
		//QVBoxLayout* layout = new QVBoxLayout(this);
		//layout->addWidget(browser);
		//setLayout(layout);
	QString filename = "C:\\Users\\joeys\\Desktop\\Temp\\TestVid.avi";
	bool vfr = false;
	int width=640;
   int height=480;
   int bitrate=10000000;
   int gop = 20;
   int fps = 25;

   // The image on which we draw the frames
   QImage frame(width,height,QImage::Format_RGB32);     // Only RGB32 is supported

   // A painter to help us draw
   QPainter painter(&frame);
   painter.setBrush(Qt::red);
   painter.setPen(Qt::white);

   // Create the encoder
   QVideoEncoder encoder;
   if(!vfr)
      encoder.createFile(filename,width,height,bitrate,gop,fps);        // Fixed frame rate
   else
      encoder.createFile(filename,width,height,bitrate*1000/fps,gop,1000);  // For variable frame rates: set the time base to e.g. 1ms (1000fps),
                                                                           // and correct the bitrate according to the expected average frame rate (fps)

   QEventLoop evt;      // we use an event loop to allow for paint events to show on-screen the generated video

   // Generate a few hundred frames
   int size=0;
   int maxframe=500;
   unsigned pts=0;
   for(unsigned i=0;i<maxframe;i++)
   {
      // Clear the frame
      painter.fillRect(frame.rect(),Qt::red);   

      // Draw a moving square
      painter.fillRect(width*i/maxframe,height*i/maxframe,30,30,Qt::blue);

      // Frame number
      painter.drawText(frame.rect(),Qt::AlignCenter,QString("Frame %1\nLast frame was %2 bytes").arg(i).arg(size));

      // Display the frame, and processes events to allow for screen redraw
      QPixmap p;
      image2Pixmap(frame,p);      
      //ui->labelVideoFrame->setPixmap(p);
      evt.processEvents();

      if(!vfr)
         size=encoder.encodeImage(frame);                      // Fixed frame rate
      else
      {
         // Variable frame rate: the pts of the first frame is 0,
         // subsequent frames slow down
         pts += sqrt(float(i));
         if(i==0)
            size=encoder.encodeImagePts(frame,0);
         else
            size=encoder.encodeImagePts(frame,pts);
      }

      printf("Encoded: %d\n",size);
   }

   encoder.close();

    }


	//QSharedPointer<QtVariantPropertyManager> propManager1_;
	//QSharedPointer<QtVariantPropertyManager> propManager2_;
	//QSharedPointer<QtVariantEditorFactory> propertyFactory_;
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
