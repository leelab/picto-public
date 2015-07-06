#include <QDebug>
#include <QApplication>
#include <QThread>
#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>

#include "BarBasePlotHandler.h"

#include "../memleakdetect.h"

#include <qwt_column_symbol.h>
#include <qwt_plot.h>
#include <qwt_plot_histogram.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_engine.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>

using namespace Picto;


namespace Picto {

//!	This class is used to scale Normalized plots.
class NormalizedScaleDraw : public QwtScaleDraw
{
public:
	//! Constructor to set our properties.
	NormalizedScaleDraw(double max)
		:max_(max)
	{
		setTickLength(QwtScaleDiv::MajorTick, 6);
		setTickLength(QwtScaleDiv::MediumTick, 3);
		setTickLength(QwtScaleDiv::MinorTick, 0);

		setSpacing(15);
	}

	//!	Overwriting labeling functionality
	virtual QwtText label(double value) const
	{
		return QString("%1").arg(value / max_);
	}
private:
	//! NormalizedScaleDraw holds onto the current total (Normalization quantity).
	double max_;
};
};

//! Constructs a new BarBasePlotHandler
BarBasePlotHandler::BarBasePlotHandler()
	: iLastZ(1), m_pDataSelectionWidget(nullptr), m_pDataSetBox(nullptr), m_pVisibilityBox(nullptr)
{

}

//! Creates or returns a widget meant to adjust dataset visibility
QWidget *BarBasePlotHandler::dataSelectionWidget()
{
	if (!m_pDataSelectionWidget)
	{
		m_pDataSelectionWidget = new QWidget();
		QHBoxLayout *layout = new QHBoxLayout(m_pDataSelectionWidget);
		m_pDataSelectionWidget->setLayout(layout);

		m_pDataSetBox = new QComboBox(m_pDataSelectionWidget);
		m_pVisibilityBox = new QCheckBox(m_pDataSelectionWidget);
		layout->addWidget(m_pDataSetBox);
		layout->addWidget(new QLabel("Visible:"));
		layout->addWidget(m_pVisibilityBox);

		connect(m_pDataSetBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedDataSetChanged(int)));
		connect(m_pVisibilityBox, SIGNAL(clicked(bool)), this, SLOT(checkboxClicked(bool)));

		foreach(QString dataSet, m_qhpHistoPlotItems.keys())
		{
			m_pDataSetBox->addItem(dataSet);
			m_pDataSetBox->setItemData(m_pDataSetBox->findText(dataSet), m_qhcHistoPlotColor[dataSet], Qt::DecorationRole);
		}
		
	}

	return m_pDataSelectionWidget;
}

//! Slot for changing currently selected dataset
void BarBasePlotHandler::selectedDataSetChanged(int /*index*/)
{
	if (m_qhbHistoPlotVisibility.contains(m_pDataSetBox->currentText()))
	{
		m_pVisibilityBox->setChecked(m_qhbHistoPlotVisibility[m_pDataSetBox->currentText()]);
	}
	else
	{
		qDebug() << "Selected uninitialized DataSet.";
	}
}

//! Slot for changing currently selected dataset visibility
void BarBasePlotHandler::checkboxClicked(bool bChecked)
{
	setVisible(m_pDataSetBox->currentText(), bChecked);
}

//! Initializes the histogram item
void BarBasePlotHandler::initializeHisto(bool bDisplayLegend, const QColor &barColor, const QColor &canvasColor, int eBarType)
{
	m_pPlot->axisScaleEngine(QwtPlot::xBottom)->setAttribute(QwtScaleEngine::Floating, true);

	if (bDisplayLegend)
	{
		QwtLegend *legend = new QwtLegend;
		legend->setDefaultItemMode(QwtLegendData::ReadOnly);
		m_pPlot->insertLegend(legend, QwtPlot::RightLegend);
	}
	else
	{
		m_pPlot->insertLegend(nullptr);
	}

	// Update the Colors and Bar Types
	updateColumns("Default", barColor, (ColumnType::ColumnType) eBarType);
	m_pPlot->canvas()->setPalette(canvasColor);
}

//! Updates the rendering properties of the columns for the indicated dataset
void BarBasePlotHandler::updateColumns(const QString &setName, const QColor &color, ColumnType::ColumnType eType)
{
	if (!m_qhpHistoPlotItems.contains(setName))
	{
		addDataSet(setName);
	}

	m_qhpHistoPlotItems[setName]->setBrush(QBrush(color));
	m_qhcHistoPlotColor[setName] = color;

	if (m_pDataSetBox && m_pDataSetBox->findText(setName) != -1)
	{
		m_pDataSetBox->setItemData(m_pDataSetBox->findText(setName), color, Qt::DecorationRole);
	}


	QPen pen(Qt::black, 0);
	QwtColumnSymbol *symbol = nullptr;

	switch (eType)
	{
	case ColumnType::COLUMN_OUTLINE:
		m_qhpHistoPlotItems[setName]->setStyle(QwtPlotHistogram::Outline);
		m_qhpHistoPlotItems[setName]->setSymbol(nullptr);
		m_qhpHistoPlotItems[setName]->setPen(pen);
		break;
	case ColumnType::COLUMN_FLAT_COLUMN:
		m_qhpHistoPlotItems[setName]->setStyle(QwtPlotHistogram::Columns);
		m_qhpHistoPlotItems[setName]->setSymbol(nullptr);
		m_qhpHistoPlotItems[setName]->setPen(pen);
		break;
	case ColumnType::COLUMN_RAISED_COLUMN:
		m_qhpHistoPlotItems[setName]->setStyle(QwtPlotHistogram::Columns);

		symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
		symbol->setFrameStyle(QwtColumnSymbol::Raised);
		symbol->setLineWidth(2);
		symbol->setPalette(QPalette(m_qhpHistoPlotItems[setName]->brush().color()));

		m_qhpHistoPlotItems[setName]->setSymbol(symbol);
		break;
	case ColumnType::COLUMN_LINES:
		m_qhpHistoPlotItems[setName]->setStyle(QwtPlotHistogram::Lines);
		m_qhpHistoPlotItems[setName]->setSymbol(nullptr);
		pen.setBrush(m_qhpHistoPlotItems[setName]->brush());
		pen.setWidth(2);
		m_qhpHistoPlotItems[setName]->setPen(pen);
		break;
	default:
		break;
	}
}

//! Sets the Y-axis scale to be normalized using the input parameters
void BarBasePlotHandler::normalizeScale(const QString & /*setName*/, double dAxisMax, double dTotalValue, const QList<double> &medium, const QList<double> &major)
{
	QwtScaleDiv scaleDiv(0, dAxisMax*dTotalValue, QList<double>(), medium, major);

	QwtScaleDraw *scaleDraw = new NormalizedScaleDraw(dTotalValue);

	m_pPlot->setAxisScaleDiv(QwtPlot::yLeft, scaleDiv);
	m_pPlot->setAxisScaleDraw(QwtPlot::yLeft, scaleDraw);
}

//! Accepts the data for the indicated dataset.
void BarBasePlotHandler::setSamples(const QString &setName, const QVector<QwtIntervalSample> &qvSamples)
{
	if (!m_qhpHistoPlotItems.contains(setName))
	{
		addDataSet(setName);
	}

	m_qhpHistoPlotItems[setName]->setSamples(new QwtIntervalSeriesData(qvSamples));
}

//! Set the scaling on the X axis
void BarBasePlotHandler::scaleAxis(double dBinSize)
{
	m_pPlot->axisScaleEngine(QwtPlot::xBottom)->setMargins(0.5*dBinSize, 0.5*dBinSize);
}

//! A slot for receiving replot commands
void BarBasePlotHandler::callReplot(const QString &setName)
{
	// Only replot if the submitted setname is visible
	if (m_qhbHistoPlotVisibility.contains(setName) && m_qhbHistoPlotVisibility[setName])
	{
		m_pPlot->replot();
	}
}

//! Adds the indicated dataset name to the list of datasets, and spawns a histogram item for it
void BarBasePlotHandler::addDataSet(const QString &setName)
{
	if (!m_qhpHistoPlotItems.contains(setName))
	{
		m_qhpHistoPlotItems[setName] = new QwtPlotHistogram(setName);
		m_qhpHistoPlotItems[setName]->attach(m_pPlot);

		iLastZ = iLastZ + 2;
		m_qhpHistoPlotItems[setName]->setZ(iLastZ);
		m_qhbHistoPlotVisibility[setName] = true;

		createErrorBars(setName);

		if (m_pDataSetBox && m_pDataSetBox->findText(setName) == -1)
		{
			m_pDataSetBox->addItem(setName);
		}
	}

	if (m_qhpHistoPlotItems.count() > 1)
	{
		dataSelectionWidget()->setVisible(true);
	}
}

//! Sets the visibility of the indicated dataset
void BarBasePlotHandler::setVisible(const QString &setName, bool visible)
{
	if (m_qhbHistoPlotVisibility.contains(setName))
	{
		m_qhpHistoPlotItems[setName]->setVisible(visible);
		m_qhbHistoPlotVisibility[setName] = visible;

		if (visible)
		{
			iLastZ = iLastZ + 2;
			m_qhpHistoPlotItems[setName]->setZ(iLastZ);

		}

		setErrorBarsVisible(true);

		m_pPlot->replot();
	}
}