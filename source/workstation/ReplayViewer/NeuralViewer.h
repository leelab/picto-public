#ifndef _NEURALVIEWER_H_
#define _NEURALVIEWER_H_

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
using namespace Picto;

class QAction;
class QToolBar;
class QComboBox;
class QLabel;
class QSlider;
class QSpinBox;
class QLineEdit;

class NeuralViewer : public QWidget
{
	Q_OBJECT
public:
	NeuralViewer(QSharedPointer<Picto::Engine::PictoEngine> engine,QWidget *parent=0);
	virtual ~NeuralViewer();
	void initialize();
	void deinitialize();
	void addLFPData(LFPDataUnitPackage &data);
	void addSpikeData(NeuralDataUnit &data);
	void replot();
	int getLatestId(){return latestNeuralDataId_;};
	int currChannel();
	int currUnit();

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