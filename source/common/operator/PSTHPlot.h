#ifndef _PSTH_PLOT_H_
#define _PSTH_PLOT_H_

#include "../common.h"
#include "NeuralDataListener.h"
#include "SamplingHistogramPlot.h"

#include <QLinkedList>

namespace Picto {

/*!	\brief A plot populated in realtime during experiments or analysis, using collected Neural Data.
 *
 *	The plot is meant for display in the Operator View.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API PSTHPlot : public SamplingHistogramPlot, public NeuralDataListener
#else
class PSTHPlot : public SamplingHistogramPlot, public NeuralDataListener
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

	double getPreFlagWindow() const;
	double getPostFlagWindow() const;

	int getChannel() const;
	int getUnit() const;

	virtual void receivePlotHandler(QSharedPointer<OperatorPlotHandler> plotHandler);

	virtual void addSpikeData(const NeuralDataUnit &data);
	virtual void setBehavioralTime(double time);

public slots:
	void alignEvent();

protected:
	virtual QSharedPointer<OperatorPlotHandler> getNewHandler();

	void clearAccumulatedData();

	//! A struct to track accumulated Spike Data
	struct DataList
	{
		DataList(){ exists = false; };
		QLinkedList<double> d;
		bool exists;
	};

	struct ActiveScan
	{
		ActiveScan(double flagTime, double binSize, double preWindow, double postWindow);

		void addSpike(int channel, int unit, double time);

		double flagTime_;
		double binSize_;
		double scanStartTime_;
		double scanEndTime_;
		int binNum_;
		QVector<QVector<QVector<int>>> spikes_;
	};

	void submitScan (const ActiveScan &scan);

	//! Retains the spike data across all channels for the last PreFlagWindow time
	QVector<QVector<DataList>> spikeData_;

	//! List of all scans in progress.
	QList<ActiveScan> activeScans_;

	//! The last neural data fitted time stamp
	double lastNeuralTime_;

};


}; //namespace Picto

#endif
