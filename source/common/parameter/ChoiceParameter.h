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

/*!	\brief A parameter for choosing between multiple options (e.g. "fast", "medium" "slow")
 *
 *	This parameter allows you to create a list of values and select between them.  Each value
 *	contains a string for naming the value and a Variant for the value itself.  You could, for
 *	example, have a "red" value which would be a QColor returning 0xFF0000.  The equality operators
 *	are clearly defined, but since there isn't an obvious way to implement them, the cmoparison
 *	operators (> and <) are undefined and will always return false.
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API ChoiceParameter : public Parameter
#else
class ChoiceParameter : public Parameter
#endif
{
	Q_OBJECT
public slots:
	void setValue(QVariant value);
	QVariant getValue();

public:
	ChoiceParameter();

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new ChoiceParameter());};
	//DataStore functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	bool addChoice(QString label, QVariant data);
	bool removeChoice(QString label);
	void setDefaultOption(QString label);

	virtual bool equalTo(Parameter& RHS);
	virtual bool equalTo(QVariant& RHS);

protected:
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QMap<QString,ChoiceParameterOption> options_;
	QString currentOption_;
	QString defaultOption_;

};


}; //namespace Picto

#endif
