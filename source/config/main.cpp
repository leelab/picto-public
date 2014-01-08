/*! \file config/main.cpp
 *	\brief This application was suppoed to configure Picto on a network
 *
 *	The original intent of this application was to configure a Picto System on a network.
 *	At the moment, all it does is run a ServerDiscoverer and print the results.  I'm
 *	not totally convinced that we need an application like this, although it might become
 *	useful if Picto reaches a wider audience.  At the moment Picto systems pretty much
 *	self-configure (although you do need to get all of the pieces running).
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */

#include <QApplication>
#include <QUdpSocket>
#include <QLabel>

#include "TestLabel.h"
#include "../common/mainmemleakdetect.h"

/*! \brief The main method for the PictoConfig application.
 *	\details This application pretty much just tells us what the Picto Server's IP Address was by using the TestLabel
 *	class.  We can build it out at some point if we can think of something useful to do with it.
 *	\sa config/main.cpp
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
int main(int argc, char *argv[])
{  
	//This will cause memory leaks to print out on exit if they're enabled
	#if defined (DETECTMEMLEAKS) && defined(_MSC_VER) && defined(_DEBUG)
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif
	QApplication a(argc, argv);

	TestLabel label;

	label.show();

	return a.exec();
}
