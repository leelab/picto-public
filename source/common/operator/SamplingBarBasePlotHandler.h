#ifndef _SAMPLING_BAR_BASE_PLOT_HANDLER_H_
#define _SAMPLING_BAR_BASE_PLOT_HANDLER_H_

#include "../common.h"
#include "BarBasePlotHandler.h"

#include <QObject>
#include <QSharedPointer>
#include <QColor>
#include <QVector>

class QwtPlotIntervalCurve;

namespace Picto {

/*! \brief A handler for plots derived from the SamplingBarBase
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
	virtual void initializeSampling();

	virtual void setErrorSamples(const QString &dataSet, const QVector<QwtIntervalSample> &qvErrorSamples);
	virtual void setErrorBarsVisible(bool bVisible);

	virtual void resetSlot();

protected:
	virtual void createErrorBars(const QString &dataSet);
	virtual void updateColor(const QString &setName, const QColor &color);
	virtual void clearPlotItems();

	//! A pointer to the SamplingBarBase's IntervalCurve used to represent error bars
	QHash<QString, QwtPlotIntervalCurve*> m_qhpErrorBars;
	//! Cached color of Errorbars, indexed by data set name
	QHash<QString, QColor> m_qhcErrorBarColor;
};

}; //namespace Picto

#endif
