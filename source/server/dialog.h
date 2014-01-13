#ifndef _DIALOG_H_
#define _DIALOG_H_

#include <QDialog>
#include "service/systemservice.h"

#include <QEvent>
#include <QKeyEvent>
#include <QSpinBox>

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
QT_END_NAMESPACE

/*!	\brief A dialog box that Users can use to install/remove/start/stop the Picto Server as a service.
 *
 *	\details The Dialog also allows the user to change the system number on which this Server operates.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
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
	void systemNumberChanged(int num);

private:
	QLabel * infoLabel;
    QPushButton * installButton;
	QPushButton * removeButton;
	QPushButton * startButton;
	QPushButton * stopButton;
	QSpinBox * systemNumberBox;
	SystemService * targetService;
};

#endif
