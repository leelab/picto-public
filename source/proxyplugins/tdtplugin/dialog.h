#ifndef _DIALOG_H_
#define _DIALOG_H_

#include <QDialog>


QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QLineEdit;
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = 0);
	QLineEdit * serverLine;
	QLineEdit * tankLine;
	QLineEdit * blockLine;

//private slots:
//private signals:
	//void initTdtServer(QString server, QString tank, QString block


private:
	QPushButton * okButton;
};

#endif
