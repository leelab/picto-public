#ifndef _TIMEDMESSAGEBOX_H_
#define _TIMEDMESSAGEBOX_H_

#include <QMessageBox>
#include <QProgressDialog>
#include "../common.h"
namespace Picto {

/*! \brief Reports time since starting the watch in as accurate units as possible on the current os.
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API TimedMessageBox : public QMessageBox
#else
class TimedMessageBox : public QMessageBox
#endif
{

public:
	//Inputing 0 means the messagebox never times out.
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
