#include <QDebug>
#include <QApplication>
#include <QThread>

#include "BarBasePlotHandler.h"

#include "../memleakdetect.h"

#include <qwt_column_symbol.h>
#include <qwt_plot.h>
#include <qwt_plot_histogram.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_engine.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>

using namespace Picto;


namespace Picto {

//!	This class is used to scale Normalized plots.
class NormalizedScaleDraw : public QwtScaleDraw
{
public:
	//! Constructor to set our properties.
	NormalizedScaleDraw(double max)
		:max_(max)
	{
		setTickLength(QwtScaleDiv::MajorTick, 6);
		setTickLength(QwtScaleDiv::MediumTick, 3);
		setTickLength(QwtScaleDiv::MinorTick, 0);

		setSpacing(15);
	}

	//!	Overwriting labeling functionality
	virtual QwtText label(double value) const
	{
		return QString("%1").arg(value / max_);
	}
private:
	//! NormalizedScaleDraw holds onto the current total (Normalization quantity).
	double max_;
};
};

BarBasePlotHandler::BarBasePlotHandler()
	: m_pHistoPlotItem(nullptr)
{

}

void BarBasePlotHandler::initializeHisto(bool bDisplayLegend, const QColor &barColor, const QColor &canvasColor, int eBarType)
{
	qDebug() << "\tBarBasePlotHandler::initializePlot called in thread: " << QThread::currentThreadId();

	m_pHistoPlotItem = new QwtPlotHistogram("Data");
	m_pHistoPlotItem->setTitle("Data");
	m_pHistoPlotItem->setStyle(QwtPlotHistogram::Columns);
	m_pHistoPlotItem->setBrush(QBrush(Qt::red));
	m_pHistoPlotItem->attach(m_pPlot);

	m_pPlot->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Floating, true);

	if (bDisplayLegend)
	{
		QwtLegend *legend = new QwtLegend;
		legend->setDefaultItemMode(QwtLegendData::ReadOnly);
		m_pPlot->insertLegend(legend, QwtPlot::RightLegend);
	}
	else
	{
		m_pPlot->insertLegend(nullptr);
	}

	// Update the Colors and Bar Types
	updateColumns(barColor, (ColumnType::ColumnType) eBarType);
	m_pPlot->canvas()->setPalette(canvasColor);
}

void BarBasePlotHandler::updateColumns(const QColor &color, ColumnType::ColumnType eType)
{
	m_pHistoPlotItem->setBrush(QBrush(color));

	QPen pen(Qt::black, 0);
	QwtColumnSymbol *symbol = nullptr;

	switch (eType)
	{
	case ColumnType::COLUMN_OUTLINE:
		m_pHistoPlotItem->setStyle(QwtPlotHistogram::Outline);
		m_pHistoPlotItem->setSymbol(nullptr);
		m_pHistoPlotItem->setPen(pen);
		break;
	case ColumnType::COLUMN_FLAT_COLUMN:
		m_pHistoPlotItem->setStyle(QwtPlotHistogram::Columns);
		m_pHistoPlotItem->setSymbol(nullptr);
		m_pHistoPlotItem->setPen(pen);
		break;
	case ColumnType::COLUMN_RAISED_COLUMN:
		m_pHistoPlotItem->setStyle(QwtPlotHistogram::Columns);

		symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
		symbol->setFrameStyle(QwtColumnSymbol::Raised);
		symbol->setLineWidth(2);
		symbol->setPalette(QPalette(m_pHistoPlotItem->brush().color()));

		m_pHistoPlotItem->setSymbol(symbol);
		break;
	case ColumnType::COLUMN_LINES:
		m_pHistoPlotItem->setStyle(QwtPlotHistogram::Lines);
		m_pHistoPlotItem->setSymbol(nullptr);
		pen.setBrush(m_pHistoPlotItem->brush());
		pen.setWidth(2);
		m_pHistoPlotItem->setPen(pen);
		break;
	default:
		break;
	}
}

void BarBasePlotHandler::normalizeScale(double dAxisMax, double dTotalValue, const QList<double> &medium, const QList<double> &major)
{

	QwtScaleDiv scaleDiv(0, dAxisMax*dTotalValue, QList<double>(), medium, major);

	QwtScaleDraw *scaleDraw = new NormalizedScaleDraw(dTotalValue);

	m_pPlot->setAxisScaleDiv(QwtPlot::yLeft, scaleDiv);
	m_pPlot->setAxisScaleDraw(QwtPlot::yLeft, scaleDraw);
}

void BarBasePlotHandler::setSamples(const QVector<QwtIntervalSample> &qvSamples)
{
	m_pHistoPlotItem->setSamples(new QwtIntervalSeriesData(qvSamples));
}

void BarBasePlotHandler::scaleAxis(double dBinSize)
{
	m_pPlot->axisScaleEngine(QwtPlot::xBottom)->setMargins(0.5*dBinSize, 0.5*dBinSize);
}

void BarBasePlotHandler::callReplot()
{
	m_pPlot->replot();
}