#ifndef _RASTER_PLOT_HANDLER_H_
#define _RASTER_PLOT_HANDLER_H_

#include "../common.h"
#include "DataViewSpecs.h"
#include "RasterBasePlotHandler.h"

#include <QObject>
#include <QSharedPointer>
#include <QColor>
#include <QVector>

class QwtPlotMarker;

namespace Picto {

	/*! \brief A handler for Raster Plots
	*	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
	*	\date 2009-2015
	*/
#if defined WIN32 || defined WINCE
	class PICTOLIB_API RasterPlotHandler : public RasterBasePlotHandler
#else
	class RasterPlotHandler : public RasterBasePlotHandler
#endif
	{
		Q_OBJECT
	public:
		RasterPlotHandler();
		virtual ~RasterPlotHandler();
			
		void setBaseline(QString datasetName, double baseline);
	};

}; //namespace Picto

#endif
