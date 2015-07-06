#ifndef _NEURAL_DATA_LISTENER_H_
#define _NEURAL_DATA_LISTENER_H_

#include "../common.h"
#include "../../common/storage/NeuralDataUnit.h"

namespace Picto {

/*!	\brief An abstract interface for items receiving NeuralData.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API NeuralDataListener
#else
class NeuralDataListener 
#endif
{
public:
	NeuralDataListener();
	virtual ~NeuralDataListener(){};

	virtual void addSpikeData(const NeuralDataUnit &data) = 0;
};


}; //namespace Picto

#endif
