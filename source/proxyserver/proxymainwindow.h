#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QTimer>
#include <QUuid>
#include "StatusLight.h"
#include "../common/network/ComponentInterface.h"


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
class ProxyMainWindow : public QMainWindow, public ComponentInterface
{
	Q_OBJECT
public:
	ProxyMainWindow();

private slots:

	void updateState();
	void enterState();
	void runState();
	void endState();
	void startConnecting();

	void setNeuralDataAcquisitionDevice(int index);
	//void startStopClient();
	bool assureDeviceRunning();

	bool isServerConnected();
	bool isSessionActive();
	void closeEvent(QCloseEvent *event);

private:
	enum ProxyState
	{
		InitState,
		WaitForConnect,
		WaitForSession,
		WaitForDevice,
		Running
	} currState_;

	enum ProxyTrigger
	{
		NoProxyTrigger,
		Connected,
		Disconnected,
		StartSessionRequest,
		DeviceStarted,
		DeviceStopped,
		SessionEnded
	} stateTrigger_;

	void createStatusLights();
	void createComboBox();
	void createButtons();
	void createLineEdits();
	void createLayout();

	void readSettings();
	void writeSettings();

	//Inherited functions from ComponentInterface
	virtual QString name();
	virtual int openDevice();
	virtual int closeDevice();	//Should make any of the calls below that are running end

	StatusLight *connectionStatus_,*sessionStatus_,*runStatus_;
	QLabel *connectionStatusLabel_, *sessionStatusLabel_, *runStatusLabel_;
	QComboBox *pluginCombo_;
	//QPushButton *startStopClientButton_;
	//QPushButton *quitButton_;
	QVBoxLayout *layout_;
	QLabel *lineEditNameLabel_;
	QLineEdit *lineEditName_;

	//QTimer *activityTimer_;

	QObject *acqPlugin_;
	QList<QObject*> acqPluginList_;

	QTimer* stateUpdateTimer_;

	bool updatingState_;

	//QString startServerMsg, stopServerMsg_;

	int port_;
};
#endif
