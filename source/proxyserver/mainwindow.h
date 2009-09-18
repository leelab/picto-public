#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>


QT_BEGIN_NAMESPACE
class QComboBox;
class QPushButton;
class QVBoxLayout;
class QEventLoop;
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
private:
	void createComboBox();
	void createButtons();
	void createLayout();
	void createTimer();

	QComboBox *pluginCombo;
	QPushButton *startStopServerButton;
	QPushButton *quitButton;
	QVBoxLayout *layout;

	QObject *acqPlugin;
	QList<QObject*> acqPluginList;

	QEventLoop *serverEventLoop;

	QString startServerMsg, stopServerMsg;
	static int a;

};
#endif
