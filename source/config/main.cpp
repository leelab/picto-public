/*! \file PictoConfig/main.cpp
 *	\brief This application was suppoed to configure Picto on a network
 *
 *	The original intent of this application was to configure a Picto System on a network.
 *	At the moment, all it does is run a ServerDiscoverer and print the results.  I'm
 *	not totally conined that we need an application like this, although it might become
 *	useful if Picto reaches a wider audience.  At the moment Picto systems pretty much
 *	self-configure (although you do need to get all of the pieces running).
 */

#include <QApplication>
#include <QUdpSocket>
#include <QLabel>

#include "TestLabel.h"
#include "../common/mainmemleakdetect.h"


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
