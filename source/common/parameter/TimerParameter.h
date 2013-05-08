#ifndef _TIMERPARAMETER_H_
#define _TIMERPARAMETER_H_

#include "../common.h"

#include "parameter.h"
#include "../controlelements/Timer.h"

namespace Picto {

/*!	\brief A parameter for containing numeric values.
 *
 *	Given our experience with Picto, it seems reasonable to limit these
 *	parameters to integer values.  If a user requires a non-integer
 *	value (e.g. 0.135 seconds), they can usually get away with changing
 *	the units (e.g. 135 milliseconds).
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API TimerParameter : public Parameter
#else
class TimerParameter : public Parameter
#endif
{
	Q_OBJECT
	Q_PROPERTY(int value READ getValue WRITE setValue)
public slots:
	void restart();

public:
	TimerParameter();
	virtual ~TimerParameter(){};

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create();

	virtual void reset();
	int getValue();
	void setValue(int val){restart();time_ = val;};

	virtual QString friendlyTypeName(){return "Timer";};
	virtual QString getUITemplate(){return "Sensor";};
	virtual QString getUIGroup(){return "Sensors";};
	//DataStore functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	Controller::Timer timer_;
	QStringList unitList_;
	int time_;


};


}; //namespace Picto

#endif
