#include <QDebug>
#include <QApplication>

#include "BarPlotPlotHandler.h"
#include "BarAxisHandler.h"

#include "../memleakdetect.h"

#include <qwt_column_symbol.h>
#include <qwt_plot.h>
#include <qwt_plot_histogram.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_engine.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>

using namespace Picto;

BarPlotPlotHandler::BarPlotPlotHandler()
	: m_pAxisHandler(nullptr)
{

}

void BarPlotPlotHandler::handleXLabels(long lLowerBound, long lUpperBound)
{
	m_pAxisHandler->rescale(lLowerBound, lUpperBound);
	m_pPlot->setAxisScaleDiv(QwtPlot::xBottom, m_pAxisHandler->getScaleDiv());
}

void BarPlotPlotHandler::initializeBarPlot(BarAxisHandler *pHandler)
{
	m_pAxisHandler = pHandler;

	m_pPlot->setAxisScaleDiv(QwtPlot::xBottom, m_pAxisHandler->getScaleDiv());
	m_pPlot->setAxisScaleDraw(QwtPlot::xBottom, m_pAxisHandler);
}