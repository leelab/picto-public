#include <QDebug>
#include <QApplication>

#include "PSTHPlotHandler.h"

#include "../memleakdetect.h"

#include <qwt_plot_marker.h>

using namespace Picto;


namespace Picto {

PSTHPlotHandler::PSTHPlotHandler()	
	: SamplingBarBasePlotHandler(), m_pEventMarker(nullptr)
{

}

void PSTHPlotHandler::initializeSampling()
{
	SamplingBarBasePlotHandler::initializeSampling();

	if (!m_pEventMarker)
	{
		m_pEventMarker = new QwtPlotMarker();
		m_pEventMarker->setLineStyle(QwtPlotMarker::VLine);
		m_pEventMarker->setLinePen(QPen(Qt::red));
		m_pEventMarker->setXValue(0);
		m_pEventMarker->attach(m_pPlot);
	}
}

}; //namespace Picto