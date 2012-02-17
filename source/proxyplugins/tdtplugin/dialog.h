#ifndef _DIALOG_H_
#define _DIALOG_H_

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QLineEdit;
QT_END_NAMESPACE


/*!\brief A Dialog used by the TDTProxyPlugin
 *
 *	To interface with a TDT system, you need to know the server, tank,
 *	and block names.  This dialog gets those from the user.  In the future
 *	it might be cool to pull them from the TDT..
 */
class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = 0);
	QLineEdit * serverLine;

private:
	QPushButton * okButton;
};

#endif
