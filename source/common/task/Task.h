#ifndef _TASK_H_
#define _TASK_H_

#include "../common.h"
#include "../storage/DataStore.h"
#include "../parameter/ParameterContainer.h"

#include <QUuid>

namespace Picto {

class Task : public DataStore
{
public:
	Task();

private:
	QUuid uuid_;
	unsigned int revision_;

	ParameterContainer parameterContainer_;
};


}; //namespace Picto

#endif
