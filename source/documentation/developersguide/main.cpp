#include <QApplication>
#include <QWebView>
#include <QUrl>
#include "../../common/mainmemleakdetect.h"

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
