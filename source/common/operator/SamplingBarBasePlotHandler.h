#ifndef _SAMPLING_BAR_BASE_PLOT_HANDLER_H_
#define _SAMPLING_BAR_BASE_PLOT_HANDLER_H_

#include "../common.h"
#include "DataViewSpecs.h"
#include "BarBasePlotHandler.h"

#include <QObject>
#include <QSharedPointer>
#include <QColor>
#include <QVector>

class QwtPlotIntervalCurve;

namespace Picto {

/*! \brief A base class for manufacturing Widgets.
 *	\note This exists because Qt requires widgets to be constructed in the UI thread.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API SamplingBarBasePlotHandler : public BarBasePlotHandler
#else
class SamplingBarBasePlotHandler : public BarBasePlotHandler
#endif
{
	Q_OBJECT
public:
	SamplingBarBasePlotHandler();
	virtual ~SamplingBarBasePlotHandler(){};

public slots:
	void initializeSampling();

	virtual void setErrorSamples(const QVector<QwtIntervalSample> &qvErrorSamples);
	virtual void setErrorBarsVisible(bool bVisible);
protected:
	//! A pointer to the SamplingBarBase's IntervalCurve used to represent error bars
	QwtPlotIntervalCurve *m_pErrorBars;
};

}; //namespace Picto

#endif
