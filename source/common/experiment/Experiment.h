#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H_

#include "common.h"

#include "../task/Task.h"

#include <vector>

namespace Picto {

class Experiment
{
public:
	Experiment();

	void addTask(Picto::Task * task);

private:
	std::vector<Picto::Task *> tasks_;
};


}; //namespace Picto

#endif
