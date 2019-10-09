#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QTimer>
#include <QUuid>
#include <QSpinBox>
#include "StatusLight.h"
#include "../common/network/ComponentInterface.h"

#include "QProgressBar.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QPushButton;
class QVBoxLayout;
class QEventLoop;
class QLineEdit;
class QLabel;
class QCheckBox;
QT_END_NAMESPACE

/*!	\brief The main window of the proxy server
 *
 *	\details This is the GUI for the Proxy application.  Since the Proxy application is
 *	so simple, this class handles the application state as well.  There are only 5 possible
 *	states:
 *		- InitState: Application starts in this state.
 *		- WaitForConnect: Application is attempting to connect to the Picto Server
 *		- WaitForSession: Application is waiting for a new session command to come in from the Picto Server.
 *		- WaitForDevice: Application is in a session and waiting for the Neural Data Acq. device handled by
 *			the plugin to start sourcing data.
 *		- Running:	Application is running a Session and sending neural data to the Picto Server.
 *
 *	It would probably be cleaner to separate out the logical state machine code from the simple UI code
 *	and that would be a good refactoring project for someone who has the time to do so.
 *	\author Mike Scudder, Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2019
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
	bool deviceIsRunning();
	bool deviceHasData();
	bool startDevice();
	void stopDevice();

	bool isServerConnected();
	bool isSessionActive();
	void closeEvent(QCloseEvent *event);

private:
	/*! \brief The logical states that a ProxyMainWindow can be in.*/
	enum ProxyState
	{
		InitState,
		WaitForConnect,
		WaitForSession,
		WaitForDevice,
		Running
	} currState_;

	/*! \brief The transitions that can occur between different logical states in the ProxyMainWindow.*/
	enum ProxyTrigger
	{
		NoProxyTrigger,
		Connected,
		Disconnected,
		ConnectDeviceRequest,
		DeviceStarted,
		DeviceStopped,
		SessionEnded
	} stateTrigger_;

	void createStatusLights();
	void createComboBox();
	void createLineEdits();
	void createSessionDBNameLabels();
	void createLayout();

	void readSettings();

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
	QSpinBox *systemNumber_;
	QLabel *systemNumLabel_;

	QLabel *sessionDBFileLabel_;
	QLabel *sessionDBFileName_;

	//QTimer *activityTimer_;

	QObject *acqPlugin_;
	QList<QObject*> acqPluginList_;

	QTimer* stateUpdateTimer_;

	bool updatingState_;

	//QString startServerMsg, stopServerMsg_;

	int port_;

private slots:
	void pluginIndexChanged(int index);
	void systemNumberChanged(int index);
	void writeSettings();

};
#endif
