#include <QApplication>
#include <QLabel>

#include "phidgets.h"

int main(int argc, char *argv[])
{
	QApplication::setDesktopSettingsAware(false);
	QApplication a(argc, argv);

	Phidgets phidgets;

	//QLabel label("HW Front User Interface Active");

	//label.show();

	a.exec();

	return 0;
}
