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
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_histogram.h>
using namespace Picto;

class QAction;
class QToolBar;
class QComboBox;
class QLabel;
class QSlider;
class QSpinBox;
class QLineEdit;

class NeuralDataViewer : public QWidget
{
	Q_OBJECT
public:
	NeuralDataViewer(QSharedPointer<Picto::Engine::PictoEngine> engine,QWidget *parent=0);
	virtual ~NeuralDataViewer();
	void initialize();
	void deinitialize();
	void addLFPData(LFPDataUnitPackage &data);
	void addSpikeData(NeuralDataUnit &data);
	void replot();
	int getLatestId(){return latestNeuralDataId_;};

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
	QVector<QwtPlotCurve*> lfpPlotCurves_;
	QwtPlotMarker* lfpCurrTimeBar_;
	QVector<QwtPlotMarker*> lfpStateChangeMarkers_;
	QVector<QVector<QwtPlotCurve*>> spikePlotCurves_;
	QwtPlotMarker* spikeCurrTimeBar_;
	QVector<QVector<int>> spikePlotRowMap_;
	QVector<QwtPlotMarker*> spikeStateChangeMarkers_;
	QString latestRunningPath_;

	int latestNeuralDataId_;

private slots:
	
	void plotChannelChanged(int index);
	void plotUnitChanged(int index);

};

#endif