#include <QTime>
#include "TestPlayback.h"
#include "../memleakdetect.h"

TestPlayback::TestPlayback()
{
	state_ = QSharedPointer<SessionState>(new SessionState());
	loader_ = QSharedPointer<SessionLoader>(new SessionLoader(state_));
	player_ = QSharedPointer<SessionPlayer>(new SessionPlayer(state_));

	connect(state_.data(),SIGNAL(propertyChanged(int,QString)),this,SLOT(propertyChanged(int,QString)));
	connect(state_.data(),SIGNAL(transitionActivated(int)),this,SLOT(transitionActivated(int)));
	connect(state_.data(),SIGNAL(framePresented(double)),this,SLOT(framePresented(double)));
	connect(state_.data(),SIGNAL(rewardSupplied(int,int)),this,SLOT(rewardSupplied(int,int)));
	connect(state_.data(),SIGNAL(signalChanged(QString,QVector<float>)),this,SLOT(signalChanged(QString,QVector<float>)));
	connect(state_.data(),SIGNAL(lfpChanged(int,double)),this,SLOT(lfpChanged(int,double)));
	connect(state_.data(),SIGNAL(spikeEvent(int,int,QVector<float>)),this,SLOT(spikeEvent(int,int,QVector<float>)));

}

void TestPlayback::Test()
{
	loader_->setFile("filename");
	//loader_->loadRun(0);
	currTime_ = 0;
	QTime timer;
	timer.start();
	for(int i=0;i<1;i++)
	{
		qDebug("\n\nStart Forward\n\n");
		while(player_->stepForward() && currTime_ < 0.08)
		{
			QCoreApplication::processEvents();
		}
		qDebug("\n\nStart Back\n\n");
		while(player_->stepBack() && currTime_ > 0.02)
		{
			QCoreApplication::processEvents();
		}
	}
	QString debugStr = QString("Elapsed Time: %1").arg(timer.elapsed());
	qDebug(debugStr.toAscii());
}

void TestPlayback::propertyChanged(int propId, QString value)
{
	QString debugStr = QString("Property Changed: ID: %1, Val: %2").arg(propId).arg(value);
	qDebug(debugStr.toAscii());
}
void TestPlayback::transitionActivated(int transId)
{
	QString debugStr = QString("Transition Activated: ID: %1").arg(transId);
	qDebug(debugStr.toAscii());
}
void TestPlayback::framePresented(double time)
{
	currTime_ = time;
	QString debugStr = QString("Frame Presented: Time: %1").arg(time);
	qDebug(debugStr.toAscii());
}
void TestPlayback::rewardSupplied(int duration,int channel)
{
	QString debugStr = QString("Reward Supplied: Duration: %1, Chan: %2").arg(duration).arg(channel);
	qDebug(debugStr.toAscii());
}
void TestPlayback::signalChanged(QString name,QVector<float> vals)
{
	QStringList strVals;
	foreach(double val,vals)
	{
		strVals.append(QString::number(val));
	}
	QString debugStr = QString("Signal Changed: Name: %1, Vals: %2").arg(name).arg(strVals.join(", "));;
	qDebug(debugStr.toAscii());
}
void TestPlayback::lfpChanged(int channel,double value)
{
	QString debugStr = QString("LFP Changed: Channel: %1, Val: %2").arg(channel).arg(value);
	qDebug(debugStr.toAscii());
}
void TestPlayback::spikeEvent(int channel, int unit, QVector<float> waveform)
{
	QStringList waveStr;
	foreach(float waveVal,waveform)
		waveStr.append(QString::number(waveVal));
	QString debugStr = QString("Spike Event: Channel: %1, Unit: %2, Waveform: %3").arg(channel).arg(unit).arg(waveStr.join(","));
	qDebug(debugStr.toAscii());
}