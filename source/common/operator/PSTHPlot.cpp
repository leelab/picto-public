#include "PSTHPlot.h"
#include "PSTHPlotHandler.h"

#include "../memleakdetect.h"

#include "../variable/NumberVariable.h"
#include "../parameter/BooleanParameter.h"
#include  "../statemachine/StateMachineElement.h"

namespace Picto {

	const QString PSTHPlot::type = "PSTH Plot";

	PSTHPlot::PSTHPlot()
	{
		AddDefinableProperty(QVariant::Double, "PreFlagWindow", 400);
		AddDefinableProperty(QVariant::Double, "PostFlagWindow", 400);

		neuron_.first = -1;
		neuron_.second = -1;
		eventID_ = -1;
	}

	/*!	\brief Constructs and returns a shared pointer to a new PSTHPlot
	*/
	QSharedPointer<Asset> PSTHPlot::Create()
	{
		return QSharedPointer<Asset>(new PSTHPlot());
	}

	QSharedPointer<OperatorPlotHandler> PSTHPlot::getNewHandler()
	{
		return QSharedPointer<OperatorPlotHandler>(new PSTHPlotHandler());
	}

	void PSTHPlot::receivePlotHandler(QSharedPointer<OperatorPlotHandler> plotHandler)
	{
		SamplingHistogramPlot::receivePlotHandler(plotHandler);

		QSharedPointer<PSTHPlot> mySharedPointer = selfPtr().objectCast<PSTHPlot>();
		QSharedPointer<NeuralDataListener> neuralSharedPointer = mySharedPointer.staticCast<NeuralDataListener>();
		QWeakPointer<NeuralDataListener> neuralWeakPointer = neuralSharedPointer.toWeakRef();


		getTaskConfig()->addNeuralDataListener(neuralWeakPointer);
	}

	//! Trigger an alignEvent, which opens a recording window
	// Legacy PSTH defined in xml - DO NOT MODIFY
	void PSTHPlot::alignEvent()
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

		activeScans_.push_back(std::move(ActiveScan(behavTime, getBinSize(), getPreFlagWindow(), getPostFlagWindow())));
		for (int chan = 0; chan < spikeData_.size(); chan++)
		{
			for (int unit = 0; unit < spikeData_[chan].size(); unit++)
			{
				foreach(double spikeTime, spikeData_[chan][unit].d)
				{
					activeScans_.back().addSpike(chan, unit, spikeTime);
				}
			}
		}
	}

	//! Clear all accumulated data.
	void PSTHPlot::clearAccumulatedData()
	{
		activeScans_.clear();
		spikeData_.clear();
		eventsData_.clear();
	}

	//! Add data from the ActiveScan into the histogram
	void PSTHPlot::submitScan(const ActiveScan &scan)
	{
		if (!checkConditions(scan))
			return;

		const QString originalSetName = currentSetName_;	

		const double binRate = 1.0 / scan.binSize_;

		for (int channel = 0; channel < scan.spikes_.size(); channel++)
		{
			if (!scan.spikes_[channel].empty())
			{
				for (int unit = 0; unit < scan.spikes_[channel].size(); unit++)
				{
					if (!scan.spikes_[channel][unit].isEmpty())
					{
						currentSetName_ = QString("Channel %1 - Unit %2").arg(channel).arg(unit);
						//setCurveOptions();
						for (int bin = 0; bin < scan.binNum_; bin++)
						{
							_submitValue(bin - scan.preBins_, binRate * scan.spikes_[channel][unit][bin]);
						}
					}
				}
			}
		}
		
		replot(currentSetName_);
		currentSetName_ = originalSetName;	
	}

	//! Convenience function to get the preFlagWindow property.
	double PSTHPlot::getPreFlagWindow() const
	{
		return propertyContainer_->getPropertyValue("PreFlagWindow").toDouble();
	}

	//! Convenience function to get the postFlagWindow property.
	double PSTHPlot::getPostFlagWindow() const
	{
		return propertyContainer_->getPropertyValue("PostFlagWindow").toDouble();
	}

	/*! \brief Updates spikeData_ with the new spike and culls data older than the PreFlagWindow.
	*	\note This implementation began as a copy from NeuralDataView::addSpikeData.
	*/
	void PSTHPlot::addSpikeData(const NeuralDataUnit &data)
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
		if (neuron.first != -1)	//not an old PSTH from XML		
		{
			int channel = neuron.first;
			int unit = neuron.second;

			if (data.getChannel() != channel || data.getUnit() != unit)
				return; //only add spikes related to this PSTH
		}


		//If there aren't enough curves slots yet, make the slots
		while (spikeData_.size() <= data.getChannel())
		{
			spikeData_.push_back(QVector<DataList>());
		}
		while (spikeData_[data.getChannel()].size() <= data.getUnit())
		{
			spikeData_[data.getChannel()].push_back(DataList());
		}
		if (!spikeData_[data.getChannel()][data.getUnit()].exists)
		{
			spikeData_[data.getChannel()][data.getUnit()].exists = true;
		}

		spikeData_[data.getChannel()][data.getUnit()].d.push_back(data.getFittedtime());

		//Find the behavTime_ - preflagwindow(converted to seconds)
		double retainMin = behavTime - 0.001 * getPreFlagWindow();

		QLinkedList<double>::iterator it;
		for (it = spikeData_[data.getChannel()][data.getUnit()].d.begin();
			it != spikeData_[data.getChannel()][data.getUnit()].d.end();
			it++)
		{
			if (*it >= retainMin)
			{
				break;
			}
		}
		spikeData_[data.getChannel()][data.getUnit()].d.erase(spikeData_[data.getChannel()][data.getUnit()].d.begin(), it);

		for (QList<ActiveScan>::iterator i = activeScans_.begin(); i != activeScans_.end();)
		{
			if (i->scanEndTime_ < data.getFittedtime())
			{
				submitScan(*i);

				i = activeScans_.erase(i);
			}
			else
			{
				i->addSpike(data.getChannel(), data.getUnit(), data.getFittedtime());
				++i;
			}
		}
	}

	//! Construct a new ActiveScan
	PSTHPlot::ActiveScan::ActiveScan(double flagTime, double binSize, double preWindow, double postWindow)
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
	void PSTHPlot::ActiveScan::addSpike(int channel, int unit, double time)
	{
		if (time >= scanEndTime_ || time < scanStartTime_)
		{
			return;
		}

		while (spikes_.size() <= channel)
		{
			spikes_.push_back(QVector<QVector<int>>());
		}
		while (spikes_[channel].size() <= unit)
		{
			spikes_[channel].push_back(QVector<int>());
			spikes_[channel].back().resize(binNum_);
		}

		int bin = int((time - scanStartTime_) / binSize_);
		++spikes_[channel][unit][bin];

	}

	QPair<int, int> PSTHPlot::getNeuron()
	{
		return neuron_;
	}
	void PSTHPlot::setNeuron(int channel, int unit)
	{
		neuron_.first = channel;
		neuron_.second = unit;
	}
	QList<int> PSTHPlot::getAlignData()
	{
		return alignElts_;
	}
	void PSTHPlot::setAlignData(QList<int> alignElts)
	{
		alignElts_ = alignElts;
	}

	QList<QPair<int, int>> PSTHPlot::getCondition()
	{
		return conditions_;
	}
	void PSTHPlot::setCondition(QList<QPair<int, int>> scriptables)
	{
		conditions_ = scriptables;
	}
	//! Trigger an alignEvent, which opens a recording window
	void PSTHPlot::alignPlot(int alignID)
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

		activeScans_.push_back(std::move(ActiveScan(behavTime, getBinSize(), getPreFlagWindow(), getPostFlagWindow())));
		if (spikeData_.size() > 0)
		{
			for (int chan = 0; chan < spikeData_.size(); chan++)
			{
				for (int unit = 0; unit < spikeData_[chan].size(); unit++)
				{
					foreach(double spikeTime, spikeData_[chan][unit].d)
					{
						activeScans_.back().addSpike(chan, unit, spikeTime);
					}
				}
			}
		}
		else
		{
			foreach(double spikeTime, eventsData_)
			{
				activeScans_.back().addSpike(0, 0, spikeTime);
			}

		}			
	}
	//! Convenience function to set the preFlagWindow property.
	void PSTHPlot::setPreFlagWindow(double flag)
	{
		propertyContainer_->setPropertyValue("PreFlagWindow", flag);
	}

	//! Convenience function to set the postFlagWindow property.
	void PSTHPlot::setPostFlagWindow(double flag)
	{
		propertyContainer_->setPropertyValue("PostFlagWindow", flag);
	}
	bool PSTHPlot::toAlign(int alignID)
	{
		bool toAlign = true;

		if ((neuron_.first == -1) && (eventID_ == -1))	//old Raster from XML		
			return true;

		//spikes are only added for relevant channel/unit or selected events		
		if (!alignElts_.contains(alignID))
			return false;

		//toAlign = checkConditions();

		return toAlign;
	}
	bool PSTHPlot::checkConditions(const ActiveScan &scan)
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
	int PSTHPlot::getEvent()
	{
		return eventID_;
	}
	void PSTHPlot::setEvent(int eventID)
	{
		eventID_ = eventID;
	}
	void PSTHPlot::addEventData(int eventID)
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
		

		if ((eventID_ == -1) || (eventID_ != eventID)) //either a  plot for neural data or a plot displaying data for a different event
			return;

		updateSpikes(behavTime);
	}
	void PSTHPlot::updateSpikes(double time)
	{
		//If there aren't enough curves slots yet, make the slots
		eventsData_.append(time);

		//Find the behavTime_ - preflagwindow(converted to seconds)
		double retainMin = time - 0.001 * getPreFlagWindow();

		QList<double>::iterator it;
		for (it = eventsData_.begin();
			it != eventsData_.end();
			it++)
		{
			if (*it >= retainMin)
			{
				break;
			}
		}
		eventsData_.erase(eventsData_.begin(), it);

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
				i->addSpike(0,0,time);
				++i;
			}
		}
	}	
	
	void PSTHPlot::addScriptCont(int scriptID)
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

	void PSTHPlot::ActiveScan::setTrialCond(QPair<int, int> scrTrialPair)
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
	void PSTHPlot::setCurveOptions()
	{
		QPair<int, int> neuron = getNeuron();
		if (neuron.first != -1)
		{
			int channel = neuron_.first;
			int unit = neuron_.second;
			currentSetName_ = QString("Channel %1 - Unit %2").arg(channel).arg(unit);
		}
		else
		{
			int event = eventID_;
			currentSetName_ = QString("Event %1").arg(event);
		}
	}
}; //namespace Picto
