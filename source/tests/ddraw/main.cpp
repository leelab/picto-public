#include <QApplication>

#include "DDrawObject.h"
#include "UpdateListener.h"

int main(int argc, char *argv[])
{
	QApplication::setDesktopSettingsAware(false);
	QApplication a(argc, argv);

	UpdateListener updateListener;

	a.exec();

	return 0;
}
