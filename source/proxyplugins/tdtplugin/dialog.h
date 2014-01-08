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
 *	\details It is possible to interface with a TDT system over the network if you know the TDT server, tank,
 *	and block names.  Once you have the server name, you can get the names of the running block/tank from that
 *	server.  This dialog gets the server name from the user.  Currently, this class is only being used when
 *	DEVELOPMENTBUILD is defined.  In general the Proxy application runs on the same PC as the TDT DAQ system, so 
 *	it can simply use "local" as the server name.
 *
 *	The dialog is useful for debugging, in case we want to access a TDT server from a Proxy that isn't
 *	running on a TDT machine.
 */
class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = 0);
	QLineEdit * serverLine;	//!< The QLineEdit where the user can enter the TDT server name.

private:
	QPushButton * okButton;
};

#endif
