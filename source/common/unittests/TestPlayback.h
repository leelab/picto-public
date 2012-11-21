
#ifndef _TEST_PLAYBACK_H
#define _TEST_PLAYBACK_H

#include <QtTest/QtTest>
#include <QSharedPointer>
#include "../playback/SessionPlayer.h"
#include "../playback/FileSessionLoader.h"
#include "../playback/SessionState.h"
#include "../globals.h"

using namespace Picto;

/*! \brief Contains tests for StateMachine objects
 *
 * 	These tests aren't particularly automated (I can't Q_COMPARE the screen
 *	output to what I think it should be), but are more of a test of system functionality.
 *
 */
class TestPlayback: public QObject
{
    Q_OBJECT

public:
	TestPlayback();

	void Test();

private:
	QSharedPointer<FileSessionLoader> loader_;
	QSharedPointer<SessionPlayer> player_;
	QSharedPointer<SessionState> state_;
	double currTime_;
private slots:
	void propertyChanged(int propId, QString value);
	void transitionActivated(int transId);
	void framePresented(double time);
	void rewardSupplied(double time,int duration,int channel);
	void signalChanged(QString name,QStringList subChanNames,QVector<float> vals);
	void lfpChanged(int channel,double value);
	void spikeEvent(int channel, int unit, QVector<float> waveform);
};


#endif