#ifndef _TIMEDMESSAGEBOX_H_
#define _TIMEDMESSAGEBOX_H_

#include <QMessageBox>
#include <QProgressDialog>
#include "../common.h"
namespace Picto {

/*! \brief This is a message box that closes itself after a preset number of seconds.
 *	
 *	\details The timeout is set in the constructor.  A ProgressBar widget is displayed showing how close we are
 *	to the timeout time.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API TimedMessageBox : public QMessageBox
#else
class TimedMessageBox : public QMessageBox
#endif
{

public:
	TimedMessageBox(int timeoutSec);
	void showEvent ( QShowEvent * event );
	void timerEvent( QTimerEvent *event);


private:
int timeout_;
bool autoClose_;
int currentTime_;
QProgressDialog* progBar_;

};

}; //namespace Picto

#endif
