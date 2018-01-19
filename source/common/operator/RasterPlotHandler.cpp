#include <QDebug>
#include <QApplication>

#include "RasterPlotHandler.h"

#include "../memleakdetect.h"

#include <qwt_plot_curve.h>

using namespace Picto;


namespace Picto {

	RasterPlotHandler::RasterPlotHandler()
		: RasterBasePlotHandler()
	{
	}

	RasterPlotHandler::~RasterPlotHandler()
	{	
	}

	void RasterPlotHandler::setBaseline(QString datasetName, double baseline)
	{
		if (!m_qhpPlotItems.contains(datasetName))
		{
			addDataSet(datasetName);
		}
				
		QwtPlotCurve *curve = (QwtPlotCurve*)m_qhpPlotItems[datasetName];
		if (curve)
		{
			curve->setBaseline(baseline);
		}		
	}
}; //namespace Picto