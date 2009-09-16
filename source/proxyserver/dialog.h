#ifndef _DIALOG_H_
#define _DIALOG_H_

#include <QDialog>
#include "service/systemservice.h"

#include <QEvent>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(SystemService * s, QWidget *parent = 0);

private slots:
	void doInstall();
	void doRemove();
	void doStart();
	void doStop();

private:
	QLabel * infoLabel;
    QPushButton * installButton;
	QPushButton * removeButton;
	QPushButton * startButton;
	QPushButton * stopButton;
	SystemService * targetService;
};

#endif
