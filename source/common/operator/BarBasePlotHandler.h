#ifndef _BAR_BASE_PLOT_HANDLER_H_
#define _BAR_BASE_PLOT_HANDLER_H_

#include "../common.h"
#include "OperatorPlotHandler.h"

#include <QObject>
#include <QSharedPointer>
#include <QColor>
#include <QVector>

class QwtPlotHistogram;
class QwtPlotCurve;
class QwtIntervalSample;


QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
class QPushButton;
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

	virtual QWidget *plotOptionsWidget();
	virtual void hideDataSelectionWidget(bool hide);

public slots:
	void initializeHisto(bool bDisplayLegend, const QColor &barColor, const QColor &canvasColor, int eBarType);

	void updateProperties(const QString &setName, const QColor &color);
	void normalizeScale(const QString &setName, double dAxisMax, double dTotalValue, const QList<double> &medium, const QList<double> &major);

	void setSamples(const QString &setName, const QVector<QwtIntervalSample> &qvSamples);
	void setPoints(const QString &setName, const QVector<QPointF> &qvPoints);
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

	virtual void resetSlot();

protected:
	//! Base function to create errorbars.  To be populated by children who use them.
	virtual void createErrorBars(const QString &) {};

	virtual void addDataSet(const QString &setName);

	virtual void updateColor(const QString &setName, const QColor &color);

	virtual void clearPlotItems();

	ColumnType::ColumnType m_ePlotColumnType;

	//! A hash of PlotItem pointers, indexed by dataset name
	QHash<QString, QwtPlotItem*> m_qhpPlotItems;
	//! A hash of HistogramPlotItem visibility, indexed by dataset name
	QHash<QString, bool> m_qhbHistoPlotVisibility;
	//! A hash of HistogramPlotItem colors, indexed by dataset name
	QHash<QString, QColor> m_qhcHistoPlotColor;

	//! Retains the last Z value applied to a plot item.
	int iLastZ;

	QWidget *m_pDataSelectionWidget;
	QWidget *m_pRightControls;
	QComboBox *m_pDataSetBox;
	QCheckBox *m_pVisibilityBox;
	QPushButton *m_pExportButton;

	bool m_bDefaultReceivedData;
};

}; //namespace Picto

#endif
