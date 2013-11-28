#ifndef _CHOICEPARAMETER_H_
#define _CHOICEPARAMETER_H_

#include "../common.h"

#include "parameter.h"

namespace Picto {

struct ChoiceParameterOption
{
	QString type;
	QVariant data;
};

/*!	\brief This Parameter is not used or properly implemented.  It was developed when the Picto design paradigm looked very different and was meant to provide multiple 
 *	options (e.g. "fast", "medium" "slow") that could show up as options in the PropertyFrame at runtime.
 *
 *	This would actually probably be a nice thing to have, but it needs to be properly implemented.  The original documentation appears below for your reference.
 *
 *	This parameter allows you to create a list of values and select between them.  Each value
 *	contains a string for naming the value and a Variant for the value itself.  You could, for
 *	example, have a "red" value which would be a QColor returning 0xFF0000.  The equality operators
 *	are clearly defined, but since there isn't an obvious way to implement them, the cmoparison
 *	operators (> and <) are undefined and will always return false.
 *
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API ChoiceParameter : public Parameter
#else
class ChoiceParameter : public Parameter
#endif
{
	Q_OBJECT
	Q_PROPERTY(QString value READ getValue WRITE setValue)
//public slots:
//	void setValue(QVariant value);
//	QVariant getValue();

public:
	ChoiceParameter();
	virtual ~ChoiceParameter(){};

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create();
	//DataStore functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	bool addChoice(QString label, QVariant data);
	bool removeChoice(QString label);

	QString getValue(){return propertyContainer_->getPropertyValue("Value").toString();};
	void setValue(QString val){propertyContainer_->setPropertyValue("Value",val);};

	virtual QString friendlyTypeName(){return "Choice";};
	
	virtual bool valuesAreValid(QString& warning = QString());
	virtual void fixValues();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QMap<QString,ChoiceParameterOption> options_;
	QString currentOption_;

};


}; //namespace Picto

#endif
