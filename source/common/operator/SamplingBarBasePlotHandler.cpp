#include <QDebug>
#include <QApplication>

#include "SamplingBarBasePlotHandler.h"

#include "../memleakdetect.h"

#include <qwt_interval_symbol.h>
#include <qwt_plot_intervalcurve.h>
#include <qwt_plot_histogram.h>

using namespace Picto;


namespace Picto {

SamplingBarBasePlotHandler::SamplingBarBasePlotHandler()
	: m_pErrorBars(nullptr)
{

}

void SamplingBarBasePlotHandler::initializeSampling()
{
	QwtIntervalSymbol *errorBar = new QwtIntervalSymbol(QwtIntervalSymbol::Bar);
	errorBar->setWidth(8);
	errorBar->setPen(QColor(Qt::blue));

	m_pErrorBars = new QwtPlotIntervalCurve("Standard Error");
	m_pErrorBars->setStyle(QwtPlotIntervalCurve::NoCurve);
	m_pErrorBars->setSymbol(errorBar);
	m_pErrorBars->setRenderHint(QwtPlotItem::RenderAntialiased, false);
	m_pErrorBars->setPen(Qt::white);
	m_pErrorBars->setBrush(QBrush(QColor(Qt::blue)));

	m_pErrorBars->attach(m_pPlot);
	m_pErrorBars->setZ(m_pHistoPlotItem->z() + 1);
}

void SamplingBarBasePlotHandler::setErrorSamples(const QVector<QwtIntervalSample> &qvErrorSamples)
{
	m_pErrorBars->setSamples(new QwtIntervalSeriesData(qvErrorSamples));
}

void SamplingBarBasePlotHandler::setErrorBarsVisible(bool bVisible)
{
	m_pErrorBars->setVisible(bVisible);
}

}; //namespace Picto