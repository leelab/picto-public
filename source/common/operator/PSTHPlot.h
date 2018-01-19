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
 *	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2017
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


	virtual void receivePlotHandler(QSharedPointer<OperatorPlotHandler> plotHandler);

	virtual void addSpikeData(const NeuralDataUnit &data);
	virtual void addEventData(int eventID);
	virtual void addScriptCont(int scriptID);

	void setPreFlagWindow(double flag);
	void setPostFlagWindow(double flag);

	virtual QPair<int, int> getNeuron();
	virtual void setNeuron(int channel, int unit);

	virtual int getEvent();
	virtual void setEvent(int eventID);

	virtual QList<int> getAlignData();
	virtual void setAlignData(QList<int> alignElts);

	virtual QList<QPair<int, int>> getCondition();
	virtual void setCondition(QList<QPair<int, int>> scriptables);

public slots:
	void alignEvent();

	void alignPlot(int alignID);

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

	//! A class to hold onto data for currently-collecting scans
	struct ActiveScan
	{
		ActiveScan(double flagTime, double binSize, double preWindow, double postWindow);

		void addSpike(int channel, int unit, double time);	
		void setTrialCond(QPair<int, int> scrTrialPair);

		//! The number of bins to track prior to the event
		int preBins_;
		//! The number of bins to track after the event
		int postBins_;

		//! The time that the alignment flag was thrown
		double flagTime_;
		//! The size of the histogram bins
		double binSize_;
		//! The start time (in seconds) of the recording window
		double scanStartTime_;
		//! The end time (in seconds) of the recording window
		double scanEndTime_;
		//! The number of bins
		int binNum_;
		//! A vector of Channel, Number, then Bin of spike counts
		QVector<QVector<QVector<int>>> spikes_;
		QList<QPair<int, int>> trialConditions_;
	};

	void submitScan (const ActiveScan &scan);
	void setCurveOptions();
	bool toAlign(int toAlign);
	void updateSpikes(double time);
	bool checkConditions(const ActiveScan &scan);

	//! Retains the spike data across all channels for the last PreFlagWindow time
	QVector<QVector<DataList>> spikeData_;

	//! List of all scans in progress.
	QList<ActiveScan> activeScans_;

	QPair<int, int> neuron_;
	QList<QPair<int, int>> conditions_;
	QList<int> alignElts_;
	int eventID_;

	//the list of spikes times for events plot
	QList<double> eventsData_;

};


}; //namespace Picto

#endif
