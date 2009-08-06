#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QDesktopWidget>
#include <QPalette>

#include <winbase.h>

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
      SignalStarted(atol(argv[1]));
  }

	return a.exec();
}
