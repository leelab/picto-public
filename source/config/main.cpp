#include <QApplication>
#include <QUdpSocket>
#include <QLabel>

#include "TestLabel.h"

int main(int argc, char *argv[])
{  
	QApplication a(argc, argv);

	TestLabel label;

	label.show();

	return a.exec();
}
