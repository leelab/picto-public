#ifndef _Raster_PLOT_H_
#define _Raster_PLOT_H_

#include "../common.h"
#include "NeuralDataListener.h"
#include "RasterBase.h"

#include <QLinkedList>
#include <QFile>

namespace Picto {

	/*!	\brief A raster plot populated in realtime during experiments or analysis, using collected Neural Data.
	* Event for Alignment and conditions can be selected such as the plot only displays corresponding spikes.
	* The conditions can be a boolean or a variable defined in the design. Make sure they already have values at the time of alignment.
	*
	*	The plot is meant for display in the Operator View.
	*	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
	*	\date 2009-2017
	*/
#if defined WIN32 || defined WINCE
	class PICTOLIB_API RasterPlot : public RasterBase, public NeuralDataListener
#else
	class RasterPlot : public RasterBase, public NeuralDataListener
#endif
	{
		Q_OBJECT
	public:
		RasterPlot();
		virtual ~RasterPlot();

		static QSharedPointer<Asset> Create();

		virtual QString friendlyTypeName(){ return "Raster Plot"; };

		//! \brief An Asset-identifying string used with AssetFactory::addAssetType
		static const QString type;

		double getPreFlagWindow() const;
		double getPostFlagWindow() const;

		void setPreFlagWindow(double flag);
		void setPostFlagWindow(double flag);

		virtual void receivePlotHandler(QSharedPointer<OperatorPlotHandler> plotHandler);

		virtual void addSpikeData(const NeuralDataUnit &data);
		virtual void addEventData(int eventID);
		virtual void addScriptCont(int scriptID);

		virtual QPair<int, int> getNeuron();
		virtual void setNeuron(int channel, int unit);

		virtual int getEvent();
		virtual void setEvent(int eventID);

		virtual QList<int> getAlignData();
		virtual void setAlignData(QList<int> alignElts);

		virtual QList<QPair<int, int>> getCondition();
		virtual void setCondition(QList<QPair<int, int>> scriptables);
	
	public slots:
		void alignPlot(int alignID);		

	protected:
		virtual QSharedPointer<OperatorPlotHandler> getNewHandler();

		void clearAccumulatedData();

		//! A class to hold onto data for currently-collecting scans
		struct ActiveScan
		{
			ActiveScan(double flagTime, double binSize, double preWindow, double postWindow);

			void addSpike(double time);
			void addMarker(int label, double time);
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
			//! A vector of Channel, Number, then times of spike counts
			QList<double> spikes_;
			QList<double> markers_;
			QList<int> labels_;
			QList<QPair<int, int>> trialConditions_;
		};

		void submitScan(const ActiveScan &scan);
		void setCurveOptions();
		bool toAlign(int toAlign);
		void updateSpikes(double time);
		void addMarkerData(int eventID, double time);
		bool checkConditions(const ActiveScan &scan);

		//! Retains the spike data for the last PreFlagWindow time
		QList<double> spikeData_;

		//! List of all scans in progress.
		QList<ActiveScan> activeScans_;

		QPair<int, int> neuron_;
		QList<QPair<int, int>> conditions_;
		QList<int> alignElts_;
		double baseline_;
		int eventID_;
		//! Retains the markers data for the last PreFlagWindow time
		QList<double> markerData_;
		QList<int> labelsData_;

		//debug
		QSharedPointer<QFile> rasterFile_;
		QSharedPointer<QTextStream> rasterFileStream_;

	};


}; //namespace Picto

#endif
