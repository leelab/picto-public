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

#ifdef WINCE
#include <winbase.h>
#endif

class BlankWindow : public QWidget
{
public:
    BlankWindow(QWidget *parent = 0) : QWidget(parent)
    {
        QWidget::setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        
        QRect screenRect = QApplication::desktop()->screenGeometry(-1);
        resize(screenRect.width(),screenRect.height());
        move(0,0);
        
        QPalette pal = palette();
        pal.setColor(QPalette::Window, Qt::black);
        setPalette(pal);
        
        setFocusPolicy(Qt::ClickFocus);
    }

    void focusInEvent(QFocusEvent *)
    {
        lower();
    }
};

int main(int argc, char *argv[])
{
	QApplication::setDesktopSettingsAware(false);
	QApplication a(argc, argv);

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
