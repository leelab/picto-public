#ifndef _PSTH_PLOT_HANDLER_H_
#define _PSTH_PLOT_HANDLER_H_

#include "../common.h"
#include "DataViewSpecs.h"
#include "SamplingBarBasePlotHandler.h"

#include <QObject>
#include <QSharedPointer>
#include <QColor>
#include <QVector>

class QwtPlotMarker;

namespace Picto {

/*! \brief A handler for PSTH Plots
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API PSTHPlotHandler : public SamplingBarBasePlotHandler
#else
class PSTHPlotHandler : public SamplingBarBasePlotHandler
#endif
{
	Q_OBJECT
public:
	PSTHPlotHandler();
	virtual ~PSTHPlotHandler(){};

public slots:
	virtual void initializeSampling();

protected:
	//! A line to mark the align event
	QwtPlotMarker* m_pEventMarker;
};

}; //namespace Picto

#endif
