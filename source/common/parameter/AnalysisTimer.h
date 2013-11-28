#ifndef _AnalysisTimer_H_
#define _AnalysisTimer_H_

#include "../common.h"

#include "AnalysisDataSource.h"
#include "../controlelements/FrameResolutionTimer.h"

namespace Picto {

/*! \brief A simple Timer for use in an Analysis.
 *	\details The timer works in frame presentation time units such that it is only
 *	ever incremented by the frame length of the latest frame.  The timer can be restarted
 *	and its value can be polled using the value property.  In many cases an AnalysisFrameData
 *	object could be used for the jobs that this timer does, but in some cases this simple
 *	single purposed object is more convenient.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisTimer : public AnalysisDataSource
#else
class AnalysisTimer : public AnalysisDataSource
#endif
{
	Q_OBJECT
	/*! \brief Sets/gets the latest value on the timer.
	 *	\details Setting the timer value causes the time of the latest frame to 
	 *	be considered equal to the input value.
	 */
	Q_PROPERTY(int value READ getValue WRITE setValue)
public slots:
	void restart();
public:
	AnalysisTimer();
	virtual ~AnalysisTimer(){};

	static QSharedPointer<Asset> Create();

	virtual void setDesignConfig(QSharedPointer<DesignConfig> designConfig);
		
	virtual void enteredScope();
	int getValue();
	/*! \brief Sets the time on this timer at the latest frame to be equal to the input value.*/
	void setValue(int val){restart();time_ = val;};

	virtual QString friendlyTypeName(){return "Timer";};
	virtual QString getUITemplate(){return "TimeDataSource";};
	virtual QString getUIGroup(){return "Sensors";};


protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QSharedPointer<Controller::FrameResolutionTimer> timer_;
	QStringList unitList_;
	int time_;
};


}; //namespace Picto

#endif
