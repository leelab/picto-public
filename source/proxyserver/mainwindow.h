#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QTimer>


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
private:
	void createComboBox();
	void createButtons();
	void createLineEdits();
	void createLayout();
	void createTimer();

	QComboBox *pluginCombo;
	QPushButton *startStopServerButton;
	QPushButton *quitButton;
	QVBoxLayout *layout;
	QLabel *lineEditNameLabel;
	QLineEdit *lineEditName;

	QObject *acqPlugin;
	QList<QObject*> acqPluginList;

	QEventLoop *serverEventLoop;

	QString startServerMsg, stopServerMsg;
	QString proxyName;

};
#endif
