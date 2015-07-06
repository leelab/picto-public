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
{

}

void SamplingBarBasePlotHandler::initializeSampling()
{

}

void SamplingBarBasePlotHandler::setErrorSamples(const QString &setName, const QVector<QwtIntervalSample> &qvErrorSamples)
{
	m_qhpErrorBars[setName]->setSamples(new QwtIntervalSeriesData(qvErrorSamples));
}

void SamplingBarBasePlotHandler::setErrorBarsVisible(bool bVisible)
{
	//Set the visibility of each errorbar to be equal to that of its corresponding plot
	foreach(QString setName, m_qhpErrorBars.keys())
	{
		m_qhpErrorBars[setName]->setVisible(bVisible && m_qhbHistoPlotVisibility[setName]);
		m_qhpErrorBars[setName]->setZ(m_qhpHistoPlotItems[setName]->z() + 1);
	}
}

void SamplingBarBasePlotHandler::createErrorBars(const QString &dataSet)
{
	QwtIntervalSymbol *errorBar = new QwtIntervalSymbol(QwtIntervalSymbol::Bar);
	errorBar->setWidth(8);
	errorBar->setPen(QColor(Qt::blue));

	m_qhpErrorBars[dataSet] = new QwtPlotIntervalCurve(dataSet + " StdErr");
	m_qhpErrorBars[dataSet]->setStyle(QwtPlotIntervalCurve::NoCurve);
	m_qhpErrorBars[dataSet]->setSymbol(errorBar);
	m_qhpErrorBars[dataSet]->setRenderHint(QwtPlotItem::RenderAntialiased, false);
	m_qhpErrorBars[dataSet]->setPen(Qt::white);
	m_qhpErrorBars[dataSet]->setBrush(QBrush(QColor(Qt::blue)));

	m_qhpErrorBars[dataSet]->attach(m_pPlot);
	m_qhpErrorBars[dataSet]->setZ(m_qhpHistoPlotItems[dataSet]->z() + 1);


}

}; //namespace Picto