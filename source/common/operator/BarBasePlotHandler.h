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


QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
QT_END_NAMESPACE

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

	virtual QWidget *dataSelectionWidget();

public slots:
	void initializeHisto(bool bDisplayLegend, const QColor &barColor, const QColor &canvasColor, int eBarType);

	void updateColumns(const QString &setName, const QColor &color, ColumnType::ColumnType eType);
	void normalizeScale(const QString &setName, double dAxisMax, double dTotalValue, const QList<double> &medium, const QList<double> &major);

	void setSamples(const QString &setName, const QVector<QwtIntervalSample> &qvSamples);
	//! A slot for receiving error samples, implemented by children who use it
	virtual void setErrorSamples(const QString &, const QVector<QwtIntervalSample>&) {};
	//! A slot for receiving error bar visibility, implemented by children who use it
	virtual void setErrorBarsVisible(bool) {};

	void scaleAxis(double dBinSize);

	//! A slot for receiving X axis labels, implemented by children who use it
	virtual void handleXLabels(long, long) {};

	void callReplot(const QString &setName);

	virtual void setVisible(const QString &setName, bool visible);

	void checkboxClicked(bool bChecked);
	void selectedDataSetChanged(int index);

protected:
	virtual void createErrorBars(const QString &) {};

	virtual void addDataSet(const QString &setName);

	//! A hash of HistogramPlotItem pointers, indexed by dataset name
	QHash<QString, QwtPlotHistogram*> m_qhpHistoPlotItems;
	//! A hash of HistogramPlotItem visibility, indexed by dataset name
	QHash<QString, bool> m_qhbHistoPlotVisibility;
	//! A hash of HistogramPlotItem colors, indexed by dataset name
	QHash<QString, QColor> m_qhcHistoPlotColor;

	//! Retains the last Z value applied to a plot item.
	int iLastZ;

	QWidget *m_pDataSelectionWidget;
	QComboBox *m_pDataSetBox;
	QCheckBox *m_pVisibilityBox;
};

}; //namespace Picto

#endif
