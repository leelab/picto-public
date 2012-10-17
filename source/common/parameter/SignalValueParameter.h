#ifndef _SIGNALVALUEPARAMETER_H_
#define _SIGNALVALUEPARAMETER_H_

#include "../common.h"

#include "parameter.h"

namespace Picto {

/*! \brief A Signal Value Parameter.
 *
 *	This parameter can be used to capture the value of an input signal.
 *	Values returned by this object are the last value available at the
 *	start of the previous frame.
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API SignalValueParameter : public Parameter
#else
class SignalValueParameter : public Parameter
#endif
{
	Q_OBJECT

public:

	static QSharedPointer<Asset> Create();
	virtual ~SignalValueParameter(){};
	static void setLatestValue(QString signal, QString subChannel, double value);
public slots:
	double getValue(QString subChannel = "");

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	SignalValueParameter();
	static QHash<QString,QHash<QString,double>> signalData_;

};


}; //namespace Picto

#endif
