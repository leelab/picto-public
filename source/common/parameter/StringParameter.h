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

#if defined WIN32 || defined WINCE
	class PICTOLIB_API StringParameter : public Parameter
#else
class StringParameter : public Parameter
#endif
{
	Q_OBJECT
public slots:
	void setValue(QVariant value);
	QVariant getValue();

public:
	StringParameter();

	static Parameter* NewParameter();

	//DataStore functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	void addOption(StringParameterOption option);

private:
	QList<StringParameterOption> options_;
};


}; //namespace Picto

#endif
