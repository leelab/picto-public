#include <QApplication>
#include <QWebView>
#include <QUrl>

int main(int argc, char *argv[])
{  
	QApplication a(argc, argv);

	QWebView developersGuideView;
	developersGuideView.load(QUrl("qrc:/documentation/index.html"));
	developersGuideView.show();

	return a.exec();
}
