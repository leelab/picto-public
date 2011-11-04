#ifndef _STRINGPARAMETER_H_
#define _STRINGPARAMETER_H_

#include "../common.h"
#include "../experiment/MediaItem.h"

#include "parameter.h"

namespace Picto {

struct StringParameterOption
{
	QString label;
	MediaItem icon;
};

/*!	\brief A parameter for storing strings
 *
 *	This parameter was never implemented.  It may turn out to be unnecessary.
 *	The parameter was intended to hold arbitrary strings, but this seems
 *	like something that would be of limited use within a state machine.
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API StringParameter : public Parameter
#else
class StringParameter : public Parameter
#endif
{
	Q_OBJECT
	Q_PROPERTY(QString value READ getValue WRITE setValue)
//public slots:
//	void setValue(QVariant value);
//	QVariant getValue();

public:
	StringParameter();

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new StringParameter());};
	//DataStore functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	void addOption(StringParameterOption option);
	QString getValue(){return propertyContainer_->getPropertyValue("Value").toString();};
	void setValue(QString val){propertyContainer_->setPropertyValue("Value",val);};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QList<StringParameterOption> options_;
};


}; //namespace Picto

#endif
