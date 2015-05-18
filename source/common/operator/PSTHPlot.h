#ifndef _PSTH_PLOT_H_
#define _PSTH_PLOT_H_

#include "../common.h"
#include "SamplingHistogramPlot.h"

namespace Picto {

/*!	\brief A plot populated in realtime during experiments or analysis, using collected Neural Data.
 *
 *	The plot is meant for display in the Operator View.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API PSTHPlot : public SamplingHistogramPlot
#else
class PSTHPlot : public SamplingHistogramPlot
#endif
{
	Q_OBJECT
public:
	PSTHPlot();
	virtual ~PSTHPlot(){};

	static QSharedPointer<Asset> Create();

	virtual QString friendlyTypeName(){return "PSTH Plot";};

	//! \brief An Asset-identifying string used with AssetFactory::addAssetType
	static const QString type;

public slots:
	void beginTrial();
	void endTrial();
	void beginRecordingWindow();
	void endRecordingWindow();
	void alignEvent();


protected:
	virtual QSharedPointer<OperatorPlotHandler> getNewHandler();
};


}; //namespace Picto

#endif
