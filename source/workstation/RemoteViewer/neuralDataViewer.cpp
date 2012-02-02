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
	latestNeuralDataId_ = -1;
	axisMin_ = 0;
	axisMax_ = NEURAL_PLOT_WINDOW_SECS;
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
}

void NeuralDataViewer::deinitialize()
{
	foreach(QwtPlotCurve* curve,lfpPlotCurves_)
	{
		if(!curve)
			continue;
		curve->detach();
		delete curve;
	}

	foreach(QVector<QwtPlotCurve*> curveVec,spikePlotCurves_)
	{
		foreach(QwtPlotCurve* curve,curveVec)
		{
			if(!curve)
				continue;
			curve->detach();
			delete curve;
		}
	}

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

	lfpPlotCurves_.clear();
	spikePlotCurves_.clear();
	spikeStateChangeMarkers_.clear();
	lfpStateChangeMarkers_.clear();
	lfpPlot_->replot();
	spikePlot_->replot();
	updatePlotOptions();
}

void NeuralDataViewer::addLFPData(LFPDataUnitPackage &data)
{
	int currDataId = data.getDataID();
	if(currDataId > latestNeuralDataId_)
		latestNeuralDataId_ = currDataId;
	while(lfpPlotCurves_.size() <= data.getChannel())
	{
		lfpPlotCurves_.push_back(NULL);
		updatePlotOptions();
	}
	QwtPointArrayData* pointData;
	if(!lfpPlotCurves_[data.getChannel()])
	{
		lfpPlotCurves_[data.getChannel()] = new QwtPlotCurve(QString("Ch%1").arg(data.getChannel()));
		lfpPlotCurves_[data.getChannel()]->setPen(QPen(Qt::darkGreen));
		pointData = new QwtPointArrayData(QVector<double>(),QVector<double>());
		lfpPlotCurves_[data.getChannel()]->setData(pointData);
	}
	else
	{
		pointData = static_cast<QwtPointArrayData*>(lfpPlotCurves_[data.getChannel()]->data());
	}
	QVector<double> times = pointData->xData();
	QVector<double> pots = pointData->yData();
	times += data.getFittedTimesAsDoubleVec();
	pots += data.getPotentialsAsDoubleVec();
	for(int i=0;i<times.size();i++)
	{
		if(times[i] >= axisMin_)
		{
			times.remove(0,i);
			pots.remove(0,i);
			break;
		}
	}
	pointData = new QwtPointArrayData(times,pots);
	lfpPlotCurves_[data.getChannel()]->setData(pointData);
}
void NeuralDataViewer::addSpikeData(NeuralDataUnit &data)
{
	int currDataId = data.getDataID();
	if(currDataId > latestNeuralDataId_)
		latestNeuralDataId_ = currDataId;

	while(spikePlotCurves_.size() <= data.getChannel())
	{
		spikePlotCurves_.push_back(QVector<QwtPlotCurve*>());
		updatePlotOptions();
	}
	while(spikePlotCurves_[data.getChannel()].size() <= data.getUnit())
	{
		spikePlotCurves_[data.getChannel()].push_back(NULL);
	}
	QwtPointArrayData* pointData;
	if(!spikePlotCurves_[data.getChannel()][data.getUnit()])
	{
		spikePlotCurves_[data.getChannel()][data.getUnit()] = new QwtPlotCurve(QString("Ch%1 Un%2").arg(data.getChannel()).arg(data.getUnit()));
		spikePlotCurves_[data.getChannel()][data.getUnit()]->setPen(QPen(Qt::darkGreen));
		pointData = new QwtPointArrayData(QVector<double>(),QVector<double>());
		spikePlotCurves_[data.getChannel()][data.getUnit()]->setData(pointData);
		updatePlotOptions();
	}
	else
	{
		pointData = static_cast<QwtPointArrayData*>(spikePlotCurves_[data.getChannel()][data.getUnit()]->data());
	}
	QVector<double> times = pointData->xData();
	QVector<double> y = pointData->yData();
	times.push_back(data.getFittedtime()-.000001);
	times.push_back(data.getFittedtime());
	times.push_back(data.getFittedtime()+.000001);
	y.push_back(0);
	y.push_back(1);
	y.push_back(0);
	for(int i=0;i<times.size();i++)
	{
		if(times[i] >= axisMin_)
		{
			times.remove(0,i);
			y.remove(0,i);
			break;
		}
	}
	pointData = new QwtPointArrayData(times,y);
	spikePlotCurves_[data.getChannel()][data.getUnit()]->setData(pointData);
}

void NeuralDataViewer::replot()
{
	//Move plot axes with time
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
		}
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
	}
	lfpPlot_->replot();
	spikePlot_->replot();
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
	spikePlot_ = new QwtPlot(QwtText("Spike Plot"),this);
	spikePlot_->setCanvasBackground(QBrush(Qt::black));
	spikePlot_->insertLegend(new QwtLegend(this),QwtPlot::BottomLegend);
	spikeCurrTimeBar_ = new QwtPlotMarker();
	spikeCurrTimeBar_->setLineStyle(QwtPlotMarker::VLine);
	spikeCurrTimeBar_->setLinePen(QPen(Qt::red));
	spikeCurrTimeBar_->attach(spikePlot_);

	//Plot layout
	QVBoxLayout* plotOpsLayout = new QVBoxLayout;
	QVBoxLayout* plotWinLayout = new QVBoxLayout;
	QHBoxLayout* plotLayout = new QHBoxLayout;
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
	QHash<int,bool> channels_;
	//Build up list of channels according to lfpPlotCurves_ and spikePlotCurves_
	for(int i=0;i<lfpPlotCurves_.size();i++)
	{
		if(lfpPlotCurves_[i])
		{
			channels_[i] = true;
		}
	}
	for(int i=0;i<spikePlotCurves_.size();i++)
	{
		if(spikePlotCurves_[i].size())
		{
			channels_[i] = true;
		}
	}

	//Add the channels to the combobox
	for(QHash<int,bool>::iterator it = channels_.begin();it != channels_.end();it++)
	{
		channelBox_->addItem(QString("Channel %1").arg(it.key()),it.key());
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
	//Detach current curve from plot
	foreach(QwtPlotCurve* curve,lfpPlotCurves_)
	{
		if(curve)
			curve->detach();
	}

	if(spikePlotCurves_.size() <= ch)
	{
		lfpPlot_->replot();
		spikePlot_->replot();
		return;
	}

	for(int i=0;i<spikePlotCurves_[ch].size();i++)
	{
		if(spikePlotCurves_[ch][i])
		{
			unitBox_->addItem(QString("Unit %1").arg(i),i);
		}
	}

	if((ch >=0) 
		&& (ch < lfpPlotCurves_.size()) 
		&& lfpPlotCurves_[ch])
		lfpPlotCurves_[ch]->attach(lfpPlot_);

	//This will take care of resetting the spike plot for the
	//case where channel changes but unit index doesn't change.
	plotUnitChanged(unitBox_->currentIndex());
	lfpPlot_->replot();
}

void NeuralDataViewer::plotUnitChanged(int)
{
	int ch = channelBox_->itemData(channelBox_->currentIndex()).toInt();
	int unit = unitBox_->itemData(unitBox_->currentIndex()).toInt();
	foreach(QVector<QwtPlotCurve*> curveVec,spikePlotCurves_)
	{
		foreach(QwtPlotCurve* curve,curveVec)
		{
			if(curve)
				curve->detach();
		}
	}
	if((ch >=0) 
		&& (ch < spikePlotCurves_.size()) 
		&& (unit >= 0)
		&& unit < spikePlotCurves_[ch].size()
		&& spikePlotCurves_[ch][unit])
	spikePlotCurves_[ch][unit]->attach(spikePlot_);
	spikePlot_->replot();
}