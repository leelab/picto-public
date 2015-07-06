#ifndef _BAR_BASE_PLOT_HANDLER_H_
#define _BAR_BASE_PLOT_HANDLER_H_

#include "../common.h"
#include "DataViewSpecs.h"
#include "OperatorPlotHandler.h"

#include <QObject>
#include <QSharedPointer>
#include <QColor>
#include <QVector>

class QwtPlotHistogram;
class QwtIntervalSample;

namespace Picto {

/*! \brief A handler for plots derived from the BarBasePlot
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API BarBasePlotHandler : public OperatorPlotHandler
#else
class BarBasePlotHandler : public OperatorPlotHandler
#endif
{
	Q_OBJECT
public:
	BarBasePlotHandler();
	virtual ~BarBasePlotHandler(){};

public slots:
	void initializeHisto(bool bDisplayLegend, const QColor &barColor, const QColor &canvasColor, int eBarType);

	void updateColumns(const QColor &color, ColumnType::ColumnType eType);
	void normalizeScale(double dAxisMax, double dTotalValue, const QList<double> &medium, const QList<double> &major);

	void setSamples(const QVector<QwtIntervalSample> &qvSamples);
	virtual void setErrorSamples(const QVector<QwtIntervalSample>&) {};
	virtual void setErrorBarsVisible(bool) {};

	void scaleAxis(double dBinSize);

	virtual void handleXLabels(long, long) {};

	void callReplot();

protected:
	QwtPlotHistogram *m_pHistoPlotItem;
};

}; //namespace Picto

#endif
