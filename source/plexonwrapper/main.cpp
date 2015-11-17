/*! \file PlexonWrapper/main.cpp
 *	\brief This application acts as a wrapper for the Plexon 32bit library
 *
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

#include <QCoreApplication>
#include <QTimer>

#include "Task.h"
#include "../common/mainmemleakdetect.h"

/*! \brief The main method for the PlexonWrapper application.
 *	\details This is a 32-bit application meant to wrap around the 32-bit Plexon libraries.  It communicates with Picto
 *	through shared memory, allowing the 64-bit application to functionally use the 32-bit library.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
int main(int argc, char *argv[])
{  
	//This will cause memory leaks to print out on exit if they're enabled
	#if defined (DETECTMEMLEAKS) && defined(_MSC_VER) && defined(_DEBUG)
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif
	QCoreApplication a(argc, argv);

	Task *task = new Task(&a);

	QObject::connect(task, SIGNAL(finished()), &a, SLOT(quit()));

	QTimer::singleShot(0, task, SLOT(run()));

	return a.exec();
}
