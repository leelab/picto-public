#ifndef _SAMPLING_BAR_PLOT_PLOT_HANDLER_H_
#define _SAMPLING_BAR_PLOT_PLOT_HANDLER_H_

#include "../common.h"
#include "DataViewSpecs.h"
#include "SamplingBarBasePlotHandler.h"

#include <QObject>
#include <QSharedPointer>
#include <QColor>
#include <QVector>

namespace Picto {

class BarAxisHandler;

/*! \brief A handler for the Sampling Bar Plot
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API SamplingBarPlotPlotHandler : public SamplingBarBasePlotHandler
#else
class SamplingBarPlotPlotHandler : public SamplingBarBasePlotHandler
#endif
{
	Q_OBJECT
public:
	SamplingBarPlotPlotHandler();
	virtual ~SamplingBarPlotPlotHandler(){};

public slots:
	void initializeSamplingBarPlot(BarAxisHandler *pHandler);
	virtual void handleXLabels(long lLowerBound, long lUpperBound);

protected:
	//! Manages the storing and rendering of axis names
	BarAxisHandler *m_pAxisHandler;
};

}; //namespace Picto

#endif
