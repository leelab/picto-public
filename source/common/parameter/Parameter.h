#ifndef _PARAMETER_H_
#define _PARAMETER_H_

#include "../common.h"
#include "../storage/DataStore.h"

namespace Picto {

class Parameter : public DataStore
{
public:
	Parameter();

	QString name();
	void setName(QString parameterName);

	QString type();

protected:
	QString type_;
	QString name_;
	bool bOperatorUI_;
};


}; //namespace Picto

#endif
