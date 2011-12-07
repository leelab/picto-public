#include "InteractiveSTDIOHandler.h"

#include <QTextStream>
#include <QApplication>
#include "../common/memleakdetect.h"


InteractiveSTDIOHandler::InteractiveSTDIOHandler()
{
}

void InteractiveSTDIOHandler::run()
{
	QTextStream stdinStream(stdin, QIODevice::ReadOnly);
	QString command;
	forever{sleep(10);}
	//do
	//{
	//	command = stdinStream.readLine();
	//} while(command.toLower() != "quit");

	QApplication::exit(0);
}
