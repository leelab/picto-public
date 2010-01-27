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

class StringParameter : Parameter
{
public:
	StringParameter();

	void addOption(StringParameterOption option);

private:
	QList<StringParameterOption> options_;
};


}; //namespace Picto

#endif
