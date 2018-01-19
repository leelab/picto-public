#include "RasterPlot.h"
#include "RasterPlotHandler.h"

#include "../memleakdetect.h"

#include "../variable/NumberVariable.h"
#include "../parameter/BooleanParameter.h"
#include  "../statemachine/StateMachineElement.h"

#include <QFileInfo>

namespace Picto {

	const QString RasterPlot::type = "Raster Plot";

	RasterPlot::RasterPlot()		
	{
		AddDefinableProperty(QVariant::Double, "PreFlagWindow", 0);
		AddDefinableProperty(QVariant::Double, "PostFlagWindow", 4000);

		neuron_.first = -1;
		neuron_.second = -1;
		eventID_ = -1;
		baseline_ = 0;
	}

	RasterPlot::~RasterPlot()
	{
		//DEBUG
		if (!rasterFile_.isNull() && rasterFile_->isOpen())
		{
			rasterFile_->close();
			rasterFile_->remove();
		}
	}

	/*!	\brief Constructs and returns a shared pointer to a new RasterPlot
	*/
	QSharedPointer<Asset> RasterPlot::Create()
	{
		return QSharedPointer<Asset>(new RasterPlot());
	}

	QSharedPointer<OperatorPlotHandler> RasterPlot::getNewHandler()
	{
		return QSharedPointer<OperatorPlotHandler>(new RasterPlotHandler());
	}

	void RasterPlot::receivePlotHandler(QSharedPointer<OperatorPlotHandler> plotHandler)
	{
		RasterBase::receivePlotHandler(plotHandler);

		QSharedPointer<RasterPlot> mySharedPointer = selfPtr().objectCast<RasterPlot>();
		QSharedPointer<NeuralDataListener> neuralSharedPointer = mySharedPointer.staticCast<NeuralDataListener>();
		QWeakPointer<NeuralDataListener> neuralWeakPointer = neuralSharedPointer.toWeakRef();

		//connect(this, SIGNAL(setBaseline(QString, double)), plotHandler.data(), SLOT(setBaseline(QString,double)));

		getTaskConfig()->addNeuralDataListener(neuralWeakPointer);

		//DEBUG		
		//create spikes output file
		QString filename = "C:/Projects/Picto/output/bin/TemporaryFiles/ExpVideos/Raster_";
		QString name = getTitle();
		filename = filename + name + ".txt";

		if (rasterFileStream_)
			rasterFileStream_.clear();

		rasterFile_ = QSharedPointer<QFile>(new QFile(filename));
		if (rasterFile_->open(QIODevice::ReadWrite))
		{
			rasterFileStream_ = QSharedPointer<QTextStream>(new QTextStream(rasterFile_.data()));
			QString header = "Start here,";
			(*rasterFileStream_) << QString(header);
			(*rasterFileStream_).flush();
		}
	}

	//! Trigger an alignEvent, which opens a recording window
	void RasterPlot::alignPlot(int alignID)
	{
		if (!toAlign(alignID))
			return;

		double behavTime = 0.0;
		if (getDesignConfig()->getFrameReader())
		{
			behavTime = getDesignConfig()->getFrameReader()->getLatestTime(); 
		}

		if (behavTime <= 0.0)
		{
			return;
		}

		//refresh plot as soon as we can
		if (activeScans_.size() > 0)
		{
			for (QList<ActiveScan>::iterator i = activeScans_.begin(); i != activeScans_.end();)
			{
				if (i->scanEndTime_ < behavTime)
				{
					submitScan(*i);

					i = activeScans_.erase(i);
				}
				else
				{
					++i;
				}
			}
		}
				
		activeScans_.push_back(std::move(ActiveScan(behavTime, 0.05, getPreFlagWindow(), getPostFlagWindow())));
		//activeScans_.push_back(std::move(ActiveScan(behavTime, 0.05, 0., 12000., baseline_)));
		foreach(double spikeTime, spikeData_)
		{
			activeScans_.back().addSpike(spikeTime);
		}
		for (int i = 0; i < markerData_.size(); i++)
		{
			activeScans_.back().addMarker(labelsData_[i], markerData_[i]);
		}	
	}

	//! Clear all accumulated data.
	void RasterPlot::clearAccumulatedData()
	{
		activeScans_.clear();
		spikeData_.clear();	
		markerData_.clear();
		labelsData_.clear();
	}

	//! Add data from the ActiveScan into the histogram
	void RasterPlot::submitScan(const ActiveScan &scan)
	{
		if (!checkConditions(scan))
			return;

		const QString originalSetName = currentSetName_;		
		setCurveOptions();

		//Debug
		if (rasterFileStream_)
		{
			QString timeStr = "End of Scan";
			(*rasterFileStream_) << QString(timeStr) << "\n";
			(*rasterFileStream_).flush();

			double tmp = scan.scanStartTime_;
			QString scanStart = "Scan start: " + QString::number(tmp) + ", SPIKES: ";
			(*rasterFileStream_) << QString(scanStart);
			(*rasterFileStream_).flush();
		}

		for (int i = 0; i < scan.spikes_.size(); i++)
		{			
			//for Debug
			if (rasterFileStream_)
			{		
				double tmp = scan.spikes_[i] - scan.scanStartTime_;
				//double tmp = (scan.spikes_[i]) * 1000.;
				QString timeStr = QString::number(tmp) + ",";

				(*rasterFileStream_) << QString(timeStr);
				(*rasterFileStream_).flush();
			}
					
			_setValue(scan.spikes_[i] - scan.scanStartTime_, 0.1 + baseline_);
		}
		//add markers to current plot	
		for (int i = 0; i < scan.markers_.size(); i++)
		{
			_addMarker(scan.labels_[i], scan.markers_[i] - scan.scanStartTime_);
		}

		replot();
		currentSetName_ = originalSetName;


		int nbRows = 50;
		if (m_pPlotHandler.objectCast<RasterBasePlotHandler>())
			nbRows = (m_pPlotHandler.objectCast<RasterBasePlotHandler>())->rowsToDisplay();

		if (baseline_ < nbRows + 1)
			baseline_ = baseline_ + 1.;
		else
		{
			baseline_ = 0.;
			//clearAccumulatedData(); //do not clear, we still need the scans data to be submitted only in a refreshed plot
			reset(); //refreshes the curve QwtPlotCurve		
		}

	}

	//! Convenience function to get the preFlagWindow property.
	double RasterPlot::getPreFlagWindow() const
	{
		return propertyContainer_->getPropertyValue("PreFlagWindow").toDouble();
	}

	//! Convenience function to get the postFlagWindow property.
	double RasterPlot::getPostFlagWindow() const
	{
		return propertyContainer_->getPropertyValue("PostFlagWindow").toDouble();
	}
	//! Convenience function to set the preFlagWindow property.
	void RasterPlot::setPreFlagWindow(double flag)
	{
		propertyContainer_->setPropertyValue("PreFlagWindow",flag);
	}

	//! Convenience function to set the postFlagWindow property.
	void RasterPlot::setPostFlagWindow(double flag)
	{
		propertyContainer_->setPropertyValue("PostFlagWindow", flag);
	}
	/*! \brief Updates spikeData_ with the new spike and culls data older than the PreFlagWindow.
	*	\note This implementation began as a copy from NeuralDataView::addSpikeData.
	*/
	void RasterPlot::addSpikeData(const NeuralDataUnit &data)
	{
		double behavTime = 0.0;
		if (getDesignConfig()->getFrameReader())
		{
			behavTime = getDesignConfig()->getFrameReader()->getLatestTime();
		}

		if (behavTime <= 0.0)
		{
			return;
		}
		
		//refresh plot as soon as we can
		if (activeScans_.size() > 0)
		{
			for (QList<ActiveScan>::iterator i = activeScans_.begin(); i != activeScans_.end();)
			{
				if (i->scanEndTime_ < behavTime)
				{
					submitScan(*i);

					i = activeScans_.erase(i);
				}
				else
				{
					++i;
				}
			}
		}

		if (eventID_ != -1) // an event plot
			return;

		QPair<int, int> neuron = getNeuron();
		if (neuron.first != -1)	//an event plot (no neural data)			
		{
			int channel = neuron.first;
			int unit = neuron.second;

			if (data.getChannel() != channel || data.getUnit() != unit)
				return; //only add spikes related to this Raster
		}
		updateSpikes(data.getFittedtime());
	}
	
	//! Construct a new ActiveScan
	RasterPlot::ActiveScan::ActiveScan(double flagTime, double binSize, double preWindow, double postWindow)
		: flagTime_(flagTime), binSize_(binSize), scanEndTime_(0.0), scanStartTime_(0.0), binNum_(0)
	{
		if (binSize_ <= 0.0)
			return;

		preBins_ = int(ceil(0.001 * preWindow / binSize_));
		postBins_ = int(ceil(0.001 * postWindow / binSize_));
		binNum_ = preBins_ + postBins_;

		scanStartTime_ = flagTime_ - preBins_ * binSize_;
		scanEndTime_ = flagTime_ + postBins_ * binSize_;
	}

	//! Add a spike to ongoing scans
	void RasterPlot::ActiveScan::addSpike(double time)
	{
		if (time >= scanEndTime_ || time < scanStartTime_)
		{
			return;
		}

		
		spikes_.append(time);
	}

	QPair<int, int> RasterPlot::getNeuron()
	{
		return neuron_;
	}
	void RasterPlot::setNeuron(int channel, int unit)
	{
		neuron_.first = channel;
		neuron_.second = unit;
	}
	QList<int> RasterPlot::getAlignData()
	{
		return alignElts_;
	}
	void RasterPlot::setAlignData(QList<int> alignElts)
	{
		alignElts_ = alignElts;
	}

	QList<QPair<int, int>> RasterPlot::getCondition()
	{
		return conditions_;
	}
	void RasterPlot::setCondition(QList<QPair<int, int>> scriptables)
	{
		conditions_ = scriptables;
	}
	void RasterPlot::setCurveOptions()
	{
		//double originalBaseline = baseline_;
		//baseline_ = scan.scanBaseline_;
		QPair<int, int> neuron = getNeuron();
		if (neuron.first != -1)
		{
			int channel = neuron_.first;
			int unit = neuron_.second;
			currentSetName_ = QString("Channel %1 - Unit %2 - Baseline %3").arg(channel).arg(unit).arg(baseline_);
		}
		else
		{
			int event = eventID_;
			currentSetName_ = QString("Event %1 - Baseline %2").arg(event).arg(baseline_);
		}
		
		//emit(setBaseline("CustomRaster", baseline_));
		if (m_pPlotHandler.objectCast<RasterPlotHandler>())
			(m_pPlotHandler.objectCast<RasterPlotHandler>())->setBaseline(currentSetName_, baseline_);

		//baseline_ = originalBaseline;

	}
	bool RasterPlot::toAlign(int alignID)
	{
		bool toAlign = true;

		if ((neuron_.first == -1) && (eventID_ == -1))		
			return false; //no old xml legacy data as raster did not exist before
		
		//spikes are only added for relevant channel/unit or selected events		
		if (!alignElts_.contains(alignID))
			return false;

		//toAlign = checkConditions();

		return toAlign;
	}
	bool RasterPlot::checkConditions(const ActiveScan &scan)
	{
		bool toSubmit = true;
		if (conditions_.size() > 0)
		{			
			QList<QPair<int, int>> trialCond = scan.trialConditions_;
			if (trialCond.size() < 1)
			{
				//no conditions were stored on the scan yet although conditions exist for this Raster Plot				
				return false;
			}

			QPair<int, int> scrPair;
			foreach(scrPair, conditions_)
			{				
				int scriptID = scrPair.first; //5499
				int scriptVal = scrPair.second; //1

				//get the trial value for this scriptable	
				QPair<int, int> scrTrialPair;
				foreach(scrTrialPair, trialCond)
				{
					int scTrialID = scrTrialPair.first; //5499
					int scTrialVal = scrTrialPair.second; //1
					//compare the values - if any of the conditions(scriptable) is not respected, do not align plots
					if ((scTrialID == scriptID) && (scriptVal != scTrialVal))
					{
						toSubmit = false;
						break;
					}
				}				
				if (!toSubmit)
					break;
			}
		}

		return toSubmit;
	}
	int RasterPlot::getEvent()
	{
		return eventID_;
	}	
	void RasterPlot::setEvent(int eventID)
	{
		eventID_ = eventID;
	}
	void RasterPlot::addEventData(int eventID)
	{	
		double behavTime = 0.0;
		if (getDesignConfig()->getFrameReader())
		{
			behavTime = getDesignConfig()->getFrameReader()->getLatestTime();
		}

		if (behavTime <= 0.0)
		{
			return;
		}

		//refresh plot as soon as we can
		if (activeScans_.size() > 0)
		{
			for (QList<ActiveScan>::iterator i = activeScans_.begin(); i != activeScans_.end();)
			{
				if (i->scanEndTime_ < behavTime)
				{
					submitScan(*i);

					i = activeScans_.erase(i);
				}
				else
				{
					++i;
				}
			}
		}
		
		//check if this is a marker event
		QSharedPointer<Asset> scAsset = getDesignConfig()->getAsset(eventID);
		QSharedPointer<StateMachineElement> elt = scAsset.staticCast<StateMachineElement>();
		if (elt && elt->getMarkerParam())
		{
			addMarkerData(eventID, behavTime);
		}
				
		if ((eventID_ == -1) || (eventID_ != eventID)) //either a  plot for neural data or a plot displaying data for a different event
			return;

		updateSpikes(behavTime);
	}
	void RasterPlot::updateSpikes(double time)
	{
		//If there aren't enough curves slots yet, make the slots
		spikeData_.append(time);

		//Find the behavTime_ - preflagwindow(converted to seconds)
		double retainMin = time - 0.001 * getPreFlagWindow();

		QList<double>::iterator it;
		for (it = spikeData_.begin();
			it != spikeData_.end();
			it++)
		{
			if (*it >= retainMin)
			{
				break;
			}
		}
		spikeData_.erase(spikeData_.begin(), it);

		if (activeScans_.size() < 1)
			return;

		for (QList<ActiveScan>::iterator i = activeScans_.begin(); i != activeScans_.end();)
		{
			if (i->scanEndTime_ < time)
			{
				submitScan(*i);

				i = activeScans_.erase(i);
			}
			else
			{
				i->addSpike(time);
				++i;
			}
		}
	}	
	void RasterPlot::addMarkerData(int eventID, double time)
	{
		markerData_.append(time);
		labelsData_.append(eventID);

		//Find the behavTime_ - preflagwindow(converted to seconds)
		double retainMin = time - 0.001 * getPreFlagWindow();

		QList<double>::iterator it;
		QList<int>::iterator lbIt;
		lbIt = labelsData_.begin();
		for (it = markerData_.begin();
			it != markerData_.end();
			it++)
		{			
			if (*it >= retainMin)
			{
				break;
			}
			lbIt++;
		}
		markerData_.erase(markerData_.begin(), it);	
		labelsData_.erase(labelsData_.begin(), lbIt);

		if (activeScans_.size() < 1)
			return;

		for (QList<ActiveScan>::iterator i = activeScans_.begin(); i != activeScans_.end();)
		{
			if (i->scanEndTime_ < time)
			{
				submitScan(*i);

				i = activeScans_.erase(i);
			}
			else
			{
				i->addMarker(eventID, time);
				++i;
			}
		}
	}
	//! Add a spike to ongoing scans
	void RasterPlot::ActiveScan::addMarker(int label, double time)
	{
		if (time >= scanEndTime_ || time < scanStartTime_)
		{
			return;
		}

		markers_.append(time);
		labels_.append(label);
	}
	void RasterPlot::addScriptCont(int scriptID)
	{
		//set Trial Values for Conditions on all past scans, unless they were already previously set
		double behavTime = 0.0;
		if (getDesignConfig()->getFrameReader())
		{
			behavTime = getDesignConfig()->getFrameReader()->getLatestTime();
		}

		if (behavTime <= 0.0)
		{
			return;
		}
		QPair<int, int> scrTrialPair;
		if (conditions_.size() > 0)
		{
			//retrieve trial values			
			QPair<int, int> scrPair;
			foreach(scrPair, conditions_)
			{
				int scriptCondID = scrPair.first; //5499				
				if (scriptCondID == scriptID)
				{
					//get the trial value for this scriptable
					int scTrialVal = -1;
					QSharedPointer<Asset> scAsset = getDesignConfig()->getAsset(scriptID);
					if (scAsset && scAsset.objectCast<NumberVariable>())
					{
						scTrialVal = scAsset.objectCast<NumberVariable>()->getValue();
					}
					else if (scAsset && scAsset.objectCast<BooleanParameter>())
					{
						scTrialVal = scAsset.objectCast<BooleanParameter>()->getValue();
					}
					//compare the values - if any of the conditions(scriptable) is not respected, do not align plots
					scrTrialPair.first = scriptID;
					scrTrialPair.second = scTrialVal;
				}
			}
		}
		
		if (activeScans_.size() > 0)
		{
			for (QList<ActiveScan>::iterator i = activeScans_.begin(); i != activeScans_.end();)
			{
				if (i->scanStartTime_ <= behavTime)
				{
					i->setTrialCond(scrTrialPair);
				}			
				++i;
			}
		}
	}

	void RasterPlot::ActiveScan::setTrialCond(QPair<int, int> scrTrialPair)
	{
		if (trialConditions_.size() < 1)
			trialConditions_.append(scrTrialPair);
		else
		{
			QPair<int, int> scrPair;
			foreach(scrPair, trialConditions_)
			{
				if (scrPair.first == scrTrialPair.first)
				{
					return; //we already have a value for this script
				}
			}
			trialConditions_.append(scrTrialPair);
		}
	}
}; //namespace Picto
