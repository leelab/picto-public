#include <QLayout>
#include "TimedMessageBox.h"
#include "../memleakdetect.h"

using namespace Picto;
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

void TimedMessageBox::timerEvent(QTimerEvent *)
{
    currentTime_++;
	progBar_->setValue(currentTime_);
    if (currentTime_>=timeout_) {
		this->done(0);
    }
}