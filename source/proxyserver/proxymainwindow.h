#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QTimer>
#include "StatusLight.h"


QT_BEGIN_NAMESPACE
class QComboBox;
class QPushButton;
class QVBoxLayout;
class QEventLoop;
class QLineEdit;
class QLabel;
QT_END_NAMESPACE

/*!	\brief The main window of the proxy server
 *
 *	The MainWindow object is the GUI for the proxy server.  It also handles a little
 *	bit of the networking (it sends out ANNOUNCE and DEPART broadcasts), and the plugins.
 */
class ProxyMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	ProxyMainWindow();

private slots:
	void setNeuralDataAcquisitionDevice(int index);
	void startStopServer();
	void checkDevStatus();
	void closeEvent(QCloseEvent *event);
	void serverActivity();
private:
	void createStatusLights();
	void createComboBox();
	void createButtons();
	void createLineEdits();
	void createLayout();
	void createTimer();

	void readSettings();
	void writeSettings();

	void announce();
	void depart();

	StatusLight *readyStatus_,*activityStatus_;
	QLabel *readyStatusLabel_, *activityStatusLabel_;
	QComboBox *pluginCombo_;
	QPushButton *startStopServerButton_;
	QPushButton *quitButton_;
	QVBoxLayout *layout_;
	QLabel *lineEditNameLabel_;
	QLineEdit *lineEditName_;

	QTimer *activityTimer_;

	QObject *acqPlugin_;
	QList<QObject*> acqPluginList_;


	QEventLoop *serverEventLoop_;

	QString startServerMsg, stopServerMsg_;
	//QString proxyName_;

	int port_;

};
#endif