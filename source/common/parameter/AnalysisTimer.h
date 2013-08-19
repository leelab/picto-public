#ifndef _AnalysisTimer_H_
#define _AnalysisTimer_H_

#include "../common.h"

#include "AnalysisDataSource.h"
#include "../controlelements/FrameResolutionTimer.h"

namespace Picto {

/*! \brief An analysis variable parameter.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisTimer : public AnalysisDataSource
#else
class AnalysisTimer : public AnalysisDataSource
#endif
{
	Q_OBJECT
	Q_PROPERTY(int value READ getValue WRITE setValue)
public slots:
	void restart();
public:
	AnalysisTimer();
	virtual ~AnalysisTimer(){};

	static QSharedPointer<Asset> Create();
		
	virtual void reset();
	int getValue();
	void setValue(int val){restart();time_ = val;};

	virtual QString friendlyTypeName(){return "Timer";};
	virtual QString getUITemplate(){return "Sensor";};
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
