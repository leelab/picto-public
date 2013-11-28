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

/*!	\brief A parameter for storing strings that is not being used.
 *
 *	There is a StringVariable for this purpose, so having a separate Parameter to do it doesn't seem to useful
 *	although one could imagine things like showing text messages to test subjects that are updatable during an experiment
 *	from the Remote Viewer.  This should be easy to implment someday if someone feels the need to do so.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
	virtual ~StringParameter(){};

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create();
	//DataStore functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	void addOption(StringParameterOption option);
	QString getValue(){return propertyContainer_->getPropertyValue("Value").toString();};
	void setValue(QString val){propertyContainer_->setPropertyValue("Value",val);};

	virtual QString friendlyTypeName(){return "String Parameter";};
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QList<StringParameterOption> options_;
};


}; //namespace Picto

#endif
