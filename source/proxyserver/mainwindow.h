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

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();

private slots:
	void setNeuralDataAcquisitionDevice(int index);
	void startStopServer();
	void checkDevStatus();
	void closeEvent(QCloseEvent *event);
	void setProxyServerName(const QString &newName);
	void serverActivity();
private:
	void createStatusLights();
	void createComboBox();
	void createButtons();
	void createLineEdits();
	void createLayout();
	void createTimer();

	StatusLight *readyStatus,*activityStatus;
	QLabel *readyStatusLabel, *activityStatusLabel;
	QComboBox *pluginCombo;
	QPushButton *startStopServerButton;
	QPushButton *quitButton;
	QVBoxLayout *layout;
	QLabel *lineEditNameLabel;
	QLineEdit *lineEditName;

	QTimer *activityTimer;

	QObject *acqPlugin;
	QList<QObject*> acqPluginList;


	QEventLoop *serverEventLoop;

	QString startServerMsg, stopServerMsg;
	QString proxyName;

	int port_;

};
#endif
