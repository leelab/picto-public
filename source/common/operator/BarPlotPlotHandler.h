#ifndef _BAR_PLOT_PLOT_HANDLER_H_
#define _BAR_PLOT_PLOT_HANDLER_H_

#include "../common.h"
#include "BarBasePlotHandler.h"

#include <QObject>
#include <QSharedPointer>
#include <QColor>
#include <QVector>

namespace Picto {

class BarAxisHandler;

/*! \brief A Handler for BarPlots.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API BarPlotPlotHandler : public BarBasePlotHandler
#else
class BarPlotPlotHandler : public BarBasePlotHandler
#endif
{
	Q_OBJECT
public:
	BarPlotPlotHandler();
	virtual ~BarPlotPlotHandler(){};

public slots:
	void initializeBarPlot(BarAxisHandler *pHandler);
	virtual void handleXLabels(long lLowerBound, long lUpperBound);

protected:
	//! Manages the storing and rendering of axis names
	BarAxisHandler *m_pAxisHandler;
};

}; //namespace Picto

#endif
