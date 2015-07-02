#ifndef _NEURAL_DATA_VIEWER_H_
#define _NEURAL_DATA_VIEWER_H_

#include "../viewer.h"
#include "../../common/engine/pictoengine.h"
#include "../../common/compositor/PixmapVisualTarget.h"
#include "../../common/compositor/VisualTargetHost.h"
#include "../../common/network/CommandChannel.h"
#include "../../common/network/ServerDiscoverer.h"
#include "../../common/storage/NeuralDataUnit.h"
#include "../../common/storage/LFPDataUnitPackage.h"

#include <QUuid>
#include <QFuture>
#include <QLinkedList>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_histogram.h>
#include <QScrollArea>
using namespace Picto;

class QAction;
class QToolBar;
class QComboBox;
class QLabel;
class QSlider;
class QSpinBox;
class QLineEdit;

/*! \brief A widget used to display live lfp and spike data in the Workstation's Remote Viewer during an experimental
 *	session.
 *	\details This widget displays two plots, one for lfp data and the other for spike data.  The data displayed on the plot
 *	is set using two drop downs to select the currently displayed channel and unit (for the spike window).  The dropdowns
 *	contain all channel/unit combinations for which data has been added to the viewer.  Beyond the actual plotted data, the
 *	plots themselves include a vertical red line representing the current experimental time.  The line moves across the
 *	plot and the plot is cleared and redrawn from the beginning whenever the current time reaches the end of the plot.
 *
 *	The Remote Viewer takes care of updating the widget by calling addLPFData() and addSpikeData() to add the latest
 *	neural data to arrive from the PictoServer.  It calls setBehavioralTime() and replot() frequently to update the plot
 *	to the latest Experimental time.
 *
 *	\note You will notice that the LFP data lags behind the red bar and jumps up in half second jumps.  The half second
 *	jump is a function of our buffering a half second of LFP data on the Proxy before sending it to the Server.  The lag
 *	is a function of the Neural Data Acquisition system being used since some only make their data available with a
 *	noticeable lag.  In particular, we have noticed that the TDT system seems to have a lag of a few seconds.  The Plexon
 *	system has a lower lag.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class NeuralDataViewer : public QScrollArea
{
	Q_OBJECT
public:
	NeuralDataViewer(QSharedPointer<Picto::Engine::PictoEngine> engine,QWidget *parent=0);
	virtual ~NeuralDataViewer();
	void initialize();
	void deinitialize();
	void addLFPData(const LFPDataUnitPackage &data);
	void addSpikeData(const NeuralDataUnit &data);
	void replot();
	/*! \brief Returns the DataId of the latest Neural data to have been written to this
	 *	viewer.
	 */
	int getLatestId() const {return latestNeuralDataId_;};
	int currChannel() const;
	int currUnit() const;

public slots:
	void setBehavioralTime(double time);
private:
	void setupUi();
	QTime neuralDataTimer_;
	int axisMin_;
	int axisMax_;

	void updatePlotOptions();

	QSharedPointer<Picto::Engine::PictoEngine> engine_;

	QComboBox *channelBox_;
	QComboBox *unitBox_;
	QwtPlot *lfpPlot_;
	QwtPlot *spikePlot_;
	//! A struct to track accumulated Neural Data
	struct DataList
	{
		DataList(){exists = false;};
		QLinkedList<QPointF> d;
		bool exists;
	};
	QVector<DataList> lfpPlotData_;
	QwtPlotMarker* lfpCurrTimeBar_;
	QVector<QwtPlotMarker*> lfpStateChangeMarkers_;
	QwtPlotCurve* lfpPlotCurve_;
	bool lfpPlotNeedsUpdate_;
	QVector<QVector<DataList>> spikePlotData_;
	QwtPlotMarker* spikeCurrTimeBar_;
	QVector<QwtPlotMarker*> spikeStateChangeMarkers_;
	QwtPlotCurve* spikePlotCurve_;
	bool spikePlotNeedsUpdate_;
	QMap<int,bool> units_;
	QString latestRunningPath_;

	int latestNeuralDataId_;
	double behavTime_;

private slots:
	
	void plotChannelChanged(int index);
	void plotUnitChanged(int index);

};

#endif