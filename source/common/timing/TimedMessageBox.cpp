#include <QLayout>
#include "TimedMessageBox.h"
#include "../memleakdetect.h"

using namespace Picto;
/*! \brief Constructs a TimedMessageBox widget.
 *	@param timeoutSec The number of seconds before the message box will automatically disappear.  0 means the timer never times out.
 */
TimedMessageBox::TimedMessageBox(int timeoutSec)
:
QMessageBox(),
timeout_(timeoutSec),
autoClose_(timeoutSec > 0),
progBar_(new QProgressDialog("Auto close dialog...","",0,timeoutSec,this))
{
	if(autoClose_)
	{
		progBar_->setWindowModality(Qt::NonModal);
		progBar_->setCancelButton(NULL);
	}
}

/*! \brief Extends QMessageBox::showEvent to initialize the MessageBox to update itself once every second in the timerEvent() function.
 */
void TimedMessageBox::showEvent ( QShowEvent * event) 
{
	QMessageBox::showEvent(event);
    currentTime_ = 0;
    if (autoClose_) {
		this->startTimer(1000);
		progBar_->move(pos().x(),pos().y()+frameGeometry().height());
		progBar_->setValue(currentTime_);
		progBar_->setMinimumDuration(0);
    }

}

/*! \brief Implements QMessageBox::timerEvent() to update the Widget's ProgressBar and close the widget once the timeout set in the 
 *	constructor has been reached.
 */
void TimedMessageBox::timerEvent(QTimerEvent *)
{
    currentTime_++;
	progBar_->setValue(currentTime_);
    if (currentTime_>=timeout_) {
		this->done(0);
    }
}