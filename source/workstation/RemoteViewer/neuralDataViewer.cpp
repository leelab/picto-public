#include "NeuralDataViewer.h"

#include "../common/globals.h"
#include "../../common/compositor/RenderingTarget.h"
#include "../../common/compositor/PCMAuralTarget.h"
#include "../../common/iodevices/AudioRewardController.h"
#include "../../common/iodevices/NullEventCodeGenerator.h"
#include "../../common/engine/XYSignalChannel.h"
#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"
#include "../../common/storage/BehavioralDataUnitPackage.h"
#include "../../common/storage/StateDataUnit.h"
#include "../../common/statemachine/statemachine.h"
#include "../../common/stimuli/cursorgraphic.h"
#include "../../common/statemachine/scene.h"
#include "../../common/storage/NeuralDataUnit.h"
#include "../../common/storage/LFPDataUnitPackage.h"
#include "../propertyframe.h"



#include <QToolBar>
#include <QAction>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringList>
#include <QComboBox>
#include <QLabel>
#include <QXmlStreamReader>
#include <QTimer>
#include <QTime>
#include <QSharedPointer>
#include <QMenu>
#include <QCloseEvent>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFileDialog>
#include <QSlider>
#include <QSpinBox>
#include <QLineEdit>
#include <QtConcurrentRun>
#include "../../common/memleakdetect.h"
#include <qwt_series_data.h>
#include <qwt_legend.h>
using namespace Picto;

#define NEURAL_PLOT_WINDOW_SECS 20
NeuralDataViewer::NeuralDataViewer(QSharedPointer<Picto::Engine::PictoEngine> engine,QWidget *parent) :
	QWidget(parent),
	engine_(engine)
{
	setupUi();
	initialize();
}

NeuralDataViewer::~NeuralDataViewer()
{
}

void NeuralDataViewer::initialize()
{
	//Reset latestNeuralDataId_ so we'll start getting new data from the first that's available
	latestNeuralDataId_ = -1;

	//No path has been running yet
	latestRunningPath_ = "";

	axisMin_ = 0;
	axisMax_ = NEURAL_PLOT_WINDOW_SECS;
	lfpPlotNeedsUpdate_ = true;
	spikePlotNeedsUpdate_ = true;
}

void NeuralDataViewer::deinitialize()
{
	lfpPlotData_.clear();
	spikePlotData_.clear();

	foreach(QwtPlotMarker* stateMarker,spikeStateChangeMarkers_)
	{
		stateMarker->detach();
		delete stateMarker;
	}

	foreach(QwtPlotMarker* stateMarker,lfpStateChangeMarkers_)
	{
		stateMarker->detach();
		delete stateMarker;
	}

	spikeStateChangeMarkers_.clear();
	lfpStateChangeMarkers_.clear();
	updatePlotOptions();
}

void NeuralDataViewer::addLFPData(LFPDataUnitPackage &data)
{
	//We set the latestNeuralDataId_ to whatever came in regardless
	//of whether its greater than the last one.  This is because the
	//dataids may not be in order.  The LFP dataid would have been
	//generated when the lfp object was first created even though it
	//contains data from 500ms later.  Also, if the server goes down and
	//restarts, data may come in out of order.
	latestNeuralDataId_ = data.getDataID();
	
	//If there aren't enough data lists yet, make the lists
	while(lfpPlotData_.size() <= data.getChannel())
	{
		lfpPlotData_.push_back(DataList());
	}
	if(!lfpPlotData_[data.getChannel()].exists)
	{
		lfpPlotData_[data.getChannel()].exists = true;
		updatePlotOptions();
	}

	//Add new data to linked list
	lfpPlotData_[data.getChannel()].d += data.getAlignedDataAsLinkedList();

	//Remove all data points from times before the beginning of the current plot
	QLinkedList<QPointF>::iterator it;
	for(it = lfpPlotData_[data.getChannel()].d.begin();
		it != lfpPlotData_[data.getChannel()].d.end(); 
		it++)
	{
		if(it->x() >= axisMin_)
		{
			break;
		}
	}
	lfpPlotData_[data.getChannel()].d.erase(lfpPlotData_[data.getChannel()].d.begin(),it);
	if(data.getChannel() == currChannel())
		lfpPlotNeedsUpdate_ = true;
}
void NeuralDataViewer::addSpikeData(NeuralDataUnit &data)
{
	latestNeuralDataId_ = data.getDataID();

	//If there aren't enough curves slots yet, make the slots
	while(spikePlotData_.size() <= data.getChannel())
	{
		spikePlotData_.push_back(QVector<DataList>());
	}
	while(spikePlotData_[data.getChannel()].size() <= data.getUnit())
	{
		spikePlotData_[data.getChannel()].push_back(DataList());
	}
	if(!spikePlotData_[data.getChannel()][data.getUnit()].exists)
	{
		spikePlotData_[data.getChannel()][data.getUnit()].exists = true;
		updatePlotOptions();
	}
	
	//Add new data to linked list
	spikePlotData_[data.getChannel()][data.getUnit()].d.push_back(QPointF(data.getFittedtime()-.000001,0));
	spikePlotData_[data.getChannel()][data.getUnit()].d.push_back(QPointF(data.getFittedtime(),1));
	spikePlotData_[data.getChannel()][data.getUnit()].d.push_back(QPointF(data.getFittedtime()+.000001,0));
	
	//Remove all data points from times before the beginning of the current plot
	QLinkedList<QPointF>::iterator it;
	for(it = spikePlotData_[data.getChannel()][data.getUnit()].d.begin();
		it != spikePlotData_[data.getChannel()][data.getUnit()].d.end(); 
		it++)
	{
		if(it->x() >= axisMin_)
		{
			break;
		}
	}
	spikePlotData_[data.getChannel()][data.getUnit()].d.erase(spikePlotData_[data.getChannel()][data.getUnit()].d.begin(),it);
	if((data.getChannel() == currChannel()) && (data.getUnit() == currUnit()))
		spikePlotNeedsUpdate_ = true;
}


void NeuralDataViewer::replot()
{
	//Move plot axes with time
	bool marksAdded_ = false;
	if(engine_)
	{
		double currTime = engine_->getLastTimeStateDataRequested();
		//Fliping scale (keeps moving up by NEURAL_PLOT_WINDOW_SECS
		if(currTime > axisMax_)
		{
			axisMin_ = axisMax_;
			axisMax_ += NEURAL_PLOT_WINDOW_SECS;
		}
		//Scrolling
		//axisMin_ = currTime-NEURAL_PLOT_WINDOW_SECS;
		//axisMax_ = currTime;

		lfpPlot_->setAxisScale(QwtPlot::xBottom,axisMin_,axisMax_);
		spikePlot_->setAxisScale(QwtPlot::xBottom,axisMin_,axisMax_);
		lfpCurrTimeBar_->setXValue(currTime);
		spikeCurrTimeBar_->setXValue(currTime);

		//Update state machine markers
		QString runningPath = engine_->getRunningPath();
		if(!runningPath.isEmpty() && runningPath != latestRunningPath_)
		{
			latestRunningPath_ = runningPath;
			//Add a mark to the spike plot
			QwtPlotMarker* newMark = new QwtPlotMarker();
			newMark->setLineStyle(QwtPlotMarker::VLine);
			newMark->setLinePen(QPen(Qt::darkBlue));
			newMark->setXValue(currTime);
			QwtText makerLabel(latestRunningPath_.split("::",QString::SkipEmptyParts).last());
			makerLabel.setColor(QColor(Qt::yellow));
			newMark->setLabel(makerLabel);
			newMark->setLabelOrientation(Qt::Vertical);
			newMark->setLabelAlignment(Qt::AlignRight);
			newMark->attach(spikePlot_);
			spikeStateChangeMarkers_.append(newMark);
			
			//Add a mark to the lfp plot
			newMark = new QwtPlotMarker();
			newMark->setLineStyle(QwtPlotMarker::VLine);
			newMark->setLinePen(QPen(Qt::darkBlue));
			newMark->setXValue(currTime);
			makerLabel.setColor(QColor(Qt::yellow));
			newMark->setLabel(makerLabel);
			newMark->setLabelOrientation(Qt::Vertical);
			newMark->setLabelAlignment(Qt::AlignRight);
			newMark->attach(lfpPlot_);
			lfpStateChangeMarkers_.append(newMark);
			marksAdded_ = true;
		}
		
		//Remove old markers from spikePlot
		for(int i=0;i<spikeStateChangeMarkers_.size();i++)
		{
			if(spikeStateChangeMarkers_[i]->xValue()>=axisMin_)
			{
				spikeStateChangeMarkers_.remove(0,i);
				break;
			}
			spikeStateChangeMarkers_[i]->detach();
			delete spikeStateChangeMarkers_[i];
			if(i == spikeStateChangeMarkers_.size()-1)
			{	//In this case we won't return through the loop and remove
				//deleted markers, so just clear everything now.
				spikeStateChangeMarkers_.clear();
			}
		}

		//Remove old markers from lfpPlot
		for(int i=0;i<lfpStateChangeMarkers_.size();i++)
		{
			if(lfpStateChangeMarkers_[i]->xValue()>=axisMin_)
			{
				lfpStateChangeMarkers_.remove(0,i);
				break;
			}
			lfpStateChangeMarkers_[i]->detach();
			delete lfpStateChangeMarkers_[i];
			if(i == lfpStateChangeMarkers_.size()-1)
			{	//In this case we won't return through the loop and remove
				//deleted markers, so just clear everything now.
				lfpStateChangeMarkers_.clear();
			}
		}
	}

	//Add data to lfp curve
	if(lfpPlotNeedsUpdate_)
	{
		int lfpPlotSize = 0;
		if((currChannel() >=0) && (currChannel() < lfpPlotData_.size()))
			lfpPlotSize = lfpPlotData_[currChannel()].d.size();
		QwtPointSeriesData* pointData;
		QVector<QPointF> dataPoints;
		if(lfpPlotSize) 
		{
			int i=0;
			foreach(QPointF point,lfpPlotData_[currChannel()].d)
			{
				if(point.x() > axisMax_)
					break;
				i++;
			}
			dataPoints.resize(i);
			i=0;
			foreach(QPointF point,lfpPlotData_[currChannel()].d)
			{
				if(point.x() > axisMax_)
					break;
				dataPoints[i] = point;
				i++;
			}
		}
		pointData = new QwtPointSeriesData(dataPoints);
		lfpPlotCurve_->setData(pointData);
		lfpPlotNeedsUpdate_ = false;
	}

	//Add data to spike curve
	if(spikePlotNeedsUpdate_)
	{
		int spikePlotSize = 0;
		if((currChannel() >=0) 
			&& (currChannel() < spikePlotData_.size()) 
			&& (currUnit()>=0) 
			&& (currUnit()<spikePlotData_[currChannel()].size()))
			spikePlotSize = spikePlotData_[currChannel()][currUnit()].d.size();
		
		QwtPointSeriesData* pointData;
		QVector<QPointF> dataPoints;
		if(spikePlotSize)
		{
			int i=0;
			foreach(QPointF point,spikePlotData_[currChannel()][currUnit()].d)
			{
				if(point.x() > axisMax_)
					break;
				i++;
			}
			dataPoints.resize(i);
			i=0;
			foreach(QPointF point,spikePlotData_[currChannel()][currUnit()].d)
			{
				if(point.x() > axisMax_)
					break;
				dataPoints[i] = point;
				i++;
			}
		}
		pointData = new QwtPointSeriesData(dataPoints);
		spikePlotCurve_->setData(pointData);
		spikePlotNeedsUpdate_ = false;
	}

	////If something changed, plot the lfp curve
	//if(marksAdded_ || lfpPlotNeedsUpdate_)
	//{
	//	lfpPlot_->replot();
	//	lfpPlotNeedsUpdate_ = false;
	//}
	////If something changed, plot the spike curve
	//if(marksAdded_ || spikePlotNeedsUpdate_)
	//{
	//	spikePlot_->replot();
	//	spikePlotNeedsUpdate_ = false;
	//}
	lfpPlot_->replot();
	spikePlot_->replot();

}

int NeuralDataViewer::currChannel()
{
	return channelBox_->itemData(channelBox_->currentIndex()).toInt();
}

int NeuralDataViewer::currUnit()
{
	return unitBox_->itemData(unitBox_->currentIndex()).toInt();
}

//! \brief Sets up the user interface portions of the GUI
void NeuralDataViewer::setupUi()
{
	//----------Plots---------------
	channelBox_ = new QComboBox;
	channelBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	connect(channelBox_,SIGNAL(currentIndexChanged(int)),this,SLOT(plotChannelChanged(int)));
	unitBox_ = new QComboBox;
	unitBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	connect(unitBox_,SIGNAL(currentIndexChanged(int)),this,SLOT(plotUnitChanged(int)));
	lfpPlot_ = new QwtPlot(QwtText("LFP Plot"),this);
	lfpPlot_->setCanvasBackground(QBrush(Qt::black));
	lfpPlot_->insertLegend(new QwtLegend(this),QwtPlot::BottomLegend);
	lfpCurrTimeBar_ = new QwtPlotMarker();
	lfpCurrTimeBar_->setLineStyle(QwtPlotMarker::VLine);
	lfpCurrTimeBar_->setLinePen(QPen(Qt::red));
	lfpCurrTimeBar_->attach(lfpPlot_);
	lfpPlotCurve_ = new QwtPlotCurve();
	lfpPlotCurve_->setPen(QPen(Qt::darkGreen));
	lfpPlotCurve_->setData(new QwtPointSeriesData(QVector<QPointF>()));
	lfpPlotCurve_->attach(lfpPlot_);
	
	spikePlot_ = new QwtPlot(QwtText("Spike Plot"),this);
	spikePlot_->setCanvasBackground(QBrush(Qt::black));
	spikePlot_->insertLegend(new QwtLegend(this),QwtPlot::BottomLegend);
	spikeCurrTimeBar_ = new QwtPlotMarker();
	spikeCurrTimeBar_->setLineStyle(QwtPlotMarker::VLine);
	spikeCurrTimeBar_->setLinePen(QPen(Qt::red));
	spikeCurrTimeBar_->attach(spikePlot_);
	spikePlotCurve_ = new QwtPlotCurve();
	spikePlotCurve_->setPen(QPen(Qt::darkGreen));
	spikePlotCurve_->setData(new QwtPointSeriesData(QVector<QPointF>()));
	spikePlotCurve_->attach(spikePlot_);


	//Plot layout
	QHBoxLayout* plotOpsLayout = new QHBoxLayout;
	QVBoxLayout* plotWinLayout = new QVBoxLayout;
	QVBoxLayout* plotLayout = new QVBoxLayout;
	plotOpsLayout->addWidget(channelBox_);
	plotOpsLayout->addWidget(unitBox_);
	plotOpsLayout->addStretch(1);
	plotWinLayout->addWidget(lfpPlot_);
	plotWinLayout->addWidget(spikePlot_);
	plotLayout->addLayout(plotOpsLayout);
	plotLayout->addLayout(plotWinLayout);
	plotLayout->setStretch(0,0);
	plotLayout->setStretch(1,1);
	
	setLayout(plotLayout);
}

void NeuralDataViewer::updatePlotOptions()
{
	int currCh = channelBox_->itemData(channelBox_->currentIndex()).toInt();
	int currUnit = unitBox_->itemData(unitBox_->currentIndex()).toInt();
	channelBox_->clear();
	unitBox_->clear();

	//Add the channels to the combobox
	QMap<int,bool> channels;
	for(int i=0;i<lfpPlotData_.size();i++)
	{
		if(lfpPlotData_[i].exists)
			channels[i] = true;
	}
	for(int i=0;i<spikePlotData_.size();i++)
	{
		if(spikePlotData_[i].size())
			channels[i] = true;
	}
	foreach(int ch,channels.keys())
	{
		channelBox_->addItem(QString("Channel %1").arg(ch),ch);
	}
		
	//Reset channel to what it was before
	for(int i=0;i<channelBox_->count();i++)
	{
		if(channelBox_->itemData(i).toInt() == currCh)
		{
			channelBox_->setCurrentIndex(i);
			break;
		}
	}
	//Just in case plotChannelChanged didn't get called, call plotChannelChanged() explicitly.
	//This will setup the plots and the unit box values.
	plotChannelChanged(channelBox_->currentIndex());

	//Reset unit to what it was before
	for(int i=0;i<unitBox_->count();i++)
	{
		if(unitBox_->itemData(i).toInt() == currUnit)
		{
			unitBox_->setCurrentIndex(i);
			break;
		}
	}
}

void NeuralDataViewer::plotChannelChanged(int)
{
	int ch = channelBox_->itemData(channelBox_->currentIndex()).toInt();
	unitBox_->clear();

	QMap<int,bool> units;
	if(spikePlotData_.size() > ch)
	{	//If spikePlotData_ is empty, we might still get called with a zero ch.
		for(int i=0;i<spikePlotData_[ch].size();i++)
		{// Create map of all units in this channel.
			if(spikePlotData_[ch][i].exists)
				units[i] = true;
		}
	}
	foreach(int unit,units.keys())
	{
		unitBox_->addItem(QString("Unit %1").arg(unit),unit);
	}

	//This will take care of resetting the spike plot for the
	//case where channel changes but unit index doesn't change.
	plotUnitChanged(unitBox_->currentIndex());
	lfpPlotNeedsUpdate_ = true;
	replot();
}

void NeuralDataViewer::plotUnitChanged(int)
{
	spikePlotNeedsUpdate_ = true;
	replot();
}