/*! \file documentation/developersguide/main.cpp
 *	\brief This application was supposed to be used to navigate the Picto documentation.
 *
 *	\details The application is really just a Qt brand web browser that looks at the regular Picto Html documentation.
 *	It is currently not working due to a simple configuration error and should be fixable by slightly adjusting the
 *	.pro file for the project.  It practice though, this is a pretty pointless application.  Just use a regular
 *	browser and look at output\documentation\main\html\index.html that way.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#include <QApplication>
#include <QWebView>
#include <QUrl>
#include "../../common/mainmemleakdetect.h"

/*! \brief The main method for the PictoDevelopersGuide application.
 *	\details This application was supposed to put the Picto documentation in a nice Picto branded window.  It hasn't been 
 *	maintained, but Picto doc can be easily viewed by simply navigating to output\documentation\main\html\index.html in
 *	any web browser.
 *	\sa documentation/developersguide/main.cpp
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

	QWebView developersGuideView;
	developersGuideView.load(QUrl("qrc:/documentation/index.html"));
	developersGuideView.show();

	return a.exec();
}
