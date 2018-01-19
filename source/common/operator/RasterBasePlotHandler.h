#ifndef _RASTER_BASE_PLOT_HANDLER_H_
#define _RASTER_BASE_PLOT_HANDLER_H_

#include "../common.h"
#include "OperatorPlotHandler.h"

#include <QObject>
#include <QSharedPointer>
#include <QColor>
#include <QVector>
#include <QLineEdit>
#include <qwt_plot_marker.h>

class QwtPlotHistogram;
class QwtPlotCurve;
class QwtIntervalSample;

class QwtPlotIntervalCurve;


QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
class QPushButton;
QT_END_NAMESPACE

namespace Picto {

/*! \brief A handler for plots derived from the RasterBasePlot
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API RasterBasePlotHandler : public OperatorPlotHandler
#else
class RasterBasePlotHandler : public OperatorPlotHandler
#endif
{
	Q_OBJECT
public:
	RasterBasePlotHandler();
	virtual ~RasterBasePlotHandler();

	virtual QWidget *plotOptionsWidget();

	int rowsToDisplay();
	void addMarkers(const QString &setName, const QVector<QPointF> &qvPoints);
	void deleteMarkers();

public slots:
	
	void initializePlot(bool bDisplayLegend, const QColor &barColor, const QColor &canvasColor);

	void updateProperties(const QString &setName, const QColor &color);
		
	void setPoints(const QString &setName, const QVector<QPointF> &qvPoints);

	void scaleAxis(double dBinSize);

	//! A slot for receiving X axis labels, implemented by children who use it
	virtual void handleXLabels(long, long) {};

	void callReplot(const QString &setName);

	void exportClicked();

	void rowsClicked(const QString &newNbOfRows);
	virtual void resetSlot();

protected:

	//! Base function to create errorbars.  To be populated by children who use them.
	//virtual void createErrorBars(const QString &) {};

	virtual void addDataSet(const QString &setName);

	virtual void updateColor(const QString &setName, const QColor &color);

	//! A hash of PlotItem pointers, indexed by dataset name
	QHash<QString, QwtPlotItem*> m_qhpPlotItems;
	//! A hash of HistogramPlotItem visibility, indexed by dataset name
	QHash<QString, bool> m_qhbPlotVisibility;
	//! A hash of HistogramPlotItem colors, indexed by dataset name
	QHash<QString, QColor> m_qhcPlotColor;

	//! Retains the last Z value applied to a plot item.
	int iLastZ;

	QWidget *m_pDataSelectionWidget;	
	QPushButton *m_pExportButton;


	QString plotTitle_;


private:
	
	QLineEdit * lineEdit_;
	int rowsNb_;
	QVector<QwtPlotMarker*> spikeStateChangeMarkers_;


};

}; //namespace Picto

#endif
