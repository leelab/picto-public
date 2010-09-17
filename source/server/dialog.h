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

/*!	\brief A diaolog box explaining the server.
 *
 *	If the server is run in a GUI environment, this dialog box pops up.  It gives the user the
 *	option to start and stop the server service.
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

private:
	QLabel * infoLabel;
    QPushButton * installButton;
	QPushButton * removeButton;
	QPushButton * startButton;
	QPushButton * stopButton;
	SystemService * targetService;
};

#endif
