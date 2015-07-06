#include "PSTHPlot.h"
#include "PSTHPlotHandler.h"

#include "../memleakdetect.h"

namespace Picto {

const QString PSTHPlot::type = "PSTH Plot";

PSTHPlot::PSTHPlot()
{
	AddDefinableProperty(QVariant::Double, "PreFlagWindow", 400);
	AddDefinableProperty(QVariant::Double, "PostFlagWindow", 400);
	AddDefinableProperty(QVariant::Int, "Channel", 0);
	AddDefinableProperty(QVariant::Int, "Unit", 0);
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
}

//! Add data from the ActiveScan into the histogram
void PSTHPlot::submitScan(const ActiveScan &scan)
{
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
					for (int bin = 0; bin < scan.binNum_; bin++)
					{
						submitValue((bin + 0.5) * scan.binSize_ - 0.001*getPreFlagWindow(), binRate * scan.spikes_[channel][unit][bin]);
					}
				}
			}
		}
	}
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

//! Convenience function to get the Channel property.
int PSTHPlot::getChannel() const
{
	return propertyContainer_->getPropertyValue("Channel").toInt();
}

//! Convenience function to get the Unit property.
int PSTHPlot::getUnit() const
{
	return propertyContainer_->getPropertyValue("Unit").toInt();
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
	scanStartTime_ = flagTime_ - 0.001 * preWindow;
	scanEndTime_ = flagTime_ + 0.001 * postWindow;

	binNum_ = int((0.001 * (preWindow + postWindow)) / binSize_) + 1;
}

//! Add a spike to ongoing scans
void PSTHPlot::ActiveScan::addSpike(int channel, int unit, double time)
{
	if (time > scanEndTime_ || time < scanStartTime_)
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

}; //namespace Picto
