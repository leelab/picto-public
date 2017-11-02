#ifndef _NEURAL_DATA_LISTENER_H_
#define _NEURAL_DATA_LISTENER_H_

#include "../common.h"
#include "../../common/storage/NeuralDataUnit.h"

namespace Picto {

/*!	\brief An abstract interface for items receiving NeuralData.
 *	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2017
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
	virtual void alignPlot(int alignID) = 0;
	virtual void addEventData(int eventID) = 0;
	virtual void addScriptCont(int scriptID) = 0;

	//Plots
	virtual double getPreFlagWindow() const = 0;
	virtual double getPostFlagWindow() const = 0;
	virtual void setPreFlagWindow(double flag) = 0;
	virtual void setPostFlagWindow(double flag) = 0;
	virtual QPair<int, int> getNeuron() = 0;
	virtual void setNeuron(int channel, int unit) = 0;
	virtual QList<int> getAlignData() = 0;
	virtual void setAlignData(QList<int> alignElts) = 0;
	virtual QList<QPair<int, int>> getCondition() = 0;
	virtual void setCondition(QList<QPair<int, int>> scriptables) = 0;
	virtual int getEvent() = 0;
	virtual void setEvent(int eventID) = 0;
};


}; //namespace Picto

#endif
