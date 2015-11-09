#include <QDebug>
#include <QApplication>
#include <QThread>
#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>

#include "BarBasePlotHandler.h"
#include "DataViewSpecs.h"

#include "../memleakdetect.h"

#include <qwt_column_symbol.h>
#include <qwt_plot.h>
#include <qwt_plot_histogram.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_engine.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_plot_curve.h>

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
	: iLastZ(1), m_pDataSelectionWidget(nullptr), m_pDataSetBox(nullptr), m_pVisibilityBox(nullptr),
	m_ePlotColumnType(ColumnType::COLUMN_MAX), m_bDefaultReceivedData(false)
{

}

//! Creates or returns a widget meant to adjust dataset visibility
QWidget *BarBasePlotHandler::plotOptionsWidget()
{
	if (!m_pDataSelectionWidget)
	{
		m_pDataSelectionWidget = new QWidget();
		QHBoxLayout *layout = new QHBoxLayout(m_pDataSelectionWidget);
		m_pDataSelectionWidget->setLayout(layout);

		m_pExportButton = new QPushButton("Export", m_pDataSelectionWidget);
		layout->addWidget(m_pExportButton, 1);
		layout->addStretch(3);

		connect(m_pExportButton, SIGNAL(clicked()), this, SLOT(exportClicked()));

		m_pRightControls = new QWidget(m_pDataSelectionWidget);
		QHBoxLayout *rightLayout = new QHBoxLayout(m_pRightControls);
		m_pRightControls->setLayout(rightLayout);
		layout->addWidget(m_pRightControls, 4);

		m_pDataSetBox = new QComboBox(m_pDataSelectionWidget);
		m_pVisibilityBox = new QCheckBox(m_pDataSelectionWidget);
		rightLayout->addWidget(m_pDataSetBox, 2);

		QLabel *boxLabel = new QLabel("Visible:", m_pDataSelectionWidget);
		boxLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		rightLayout->addWidget(boxLabel, 1);
		rightLayout->addWidget(m_pVisibilityBox, 1);

		connect(m_pDataSetBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedDataSetChanged(int)));
		connect(m_pVisibilityBox, SIGNAL(clicked(bool)), this, SLOT(checkboxClicked(bool)));

		foreach(QString dataSet, m_qhpPlotItems.keys())
		{
			m_pDataSetBox->addItem(dataSet);
			m_pDataSetBox->setItemData(m_pDataSetBox->findText(dataSet), m_qhcHistoPlotColor[dataSet], Qt::DecorationRole);
		}
		
	}

	return m_pDataSelectionWidget;
}

void BarBasePlotHandler::hideDataSelectionWidget(bool hide)
{
	if (m_pRightControls)
	{
		m_pRightControls->setVisible(!hide);
	}
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

	//Double-check and clear existing data

	if (m_ePlotColumnType != ColumnType::COLUMN_MAX && m_ePlotColumnType != eBarType)
	{
		clearPlotItems();
	}

	m_ePlotColumnType = (ColumnType::ColumnType) eBarType;

	m_qhcHistoPlotColor["Default"] = barColor;

	m_pPlot->canvas()->setPalette(canvasColor);
}

//! Clears stored plot data
void BarBasePlotHandler::resetSlot()
{
	foreach(QwtPlotItem *plotItem, m_qhpPlotItems)
	{
		switch (m_ePlotColumnType)
		{
		case ColumnType::COLUMN_OUTLINE:
		case ColumnType::COLUMN_FLAT_COLUMN:
		case ColumnType::COLUMN_RAISED_COLUMN:
		case ColumnType::COLUMN_LINES:
			((QwtPlotHistogram*)plotItem)->setSamples(new QwtIntervalSeriesData());
			break;
		case ColumnType::COLUMN_CURVE:
		case ColumnType::COLUMN_FILLED_CURVE:
			((QwtPlotCurve *)plotItem)->setData(new QwtPointSeriesData());
			break;
		default:
			qDebug() << "Undefined PlotColumnType for reset";
			break;
		}
	}
}

//! Spin through, detatch, and delete all plot items.  Usually for initializing a new PlotItem Type
void BarBasePlotHandler::clearPlotItems()
{
	QHash<QString, QwtPlotItem *>::iterator iter = m_qhpPlotItems.begin();
	while (iter != m_qhpPlotItems.end())
	{
		(*iter)->detach();
		delete *iter;
		iter = m_qhpPlotItems.erase(iter);
	}

	m_qhbHistoPlotVisibility.clear();
	m_qhcHistoPlotColor.clear();
	iLastZ = 1;
	m_pDataSetBox->clear();
	m_bDefaultReceivedData = false;

	hideDataSelectionWidget(true);
}

//! Updates the rendering properties of the columns for the indicated dataset
void BarBasePlotHandler::updateProperties(const QString &setName, const QColor &color)
{
	if (!m_qhpPlotItems.contains(setName))
	{
		addDataSet(setName);
	}

	updateColor(setName, color);
}

//! Virtual function to update the color of the named dataset
void BarBasePlotHandler::updateColor(const QString &setName, const QColor &color)
{
	QPen pen(Qt::black, 0);
	switch (m_ePlotColumnType)
	{
	case ColumnType::COLUMN_OUTLINE:
	{
		QwtPlotHistogram *histo = (QwtPlotHistogram*)m_qhpPlotItems[setName];
		histo->setBrush(QBrush(color));
		histo->setStyle(QwtPlotHistogram::Outline);
		histo->setSymbol(nullptr);
		histo->setPen(pen);
		break;
	}
	case ColumnType::COLUMN_FLAT_COLUMN:
	{
		QwtPlotHistogram *histo = (QwtPlotHistogram*)m_qhpPlotItems[setName];
		histo->setBrush(QBrush(color));
		histo->setStyle(QwtPlotHistogram::Columns);
		histo->setSymbol(nullptr);
		histo->setPen(pen);
		break;
	}
	case ColumnType::COLUMN_RAISED_COLUMN:
	{
		QwtPlotHistogram *histo = (QwtPlotHistogram*)m_qhpPlotItems[setName];
		histo->setBrush(QBrush(color));
		histo->setStyle(QwtPlotHistogram::Columns);

		QwtColumnSymbol *symbol = new QwtColumnSymbol(QwtColumnSymbol::Box);
		symbol->setFrameStyle(QwtColumnSymbol::Raised);
		symbol->setLineWidth(2);
		symbol->setPalette(QPalette(histo->brush().color()));

		histo->setSymbol(symbol);
		break;
	}
	case ColumnType::COLUMN_LINES:
	{
		QwtPlotHistogram *histo = (QwtPlotHistogram*)m_qhpPlotItems[setName];
		histo->setBrush(QBrush(color));
		histo->setStyle(QwtPlotHistogram::Lines);
		histo->setSymbol(nullptr);
		pen.setBrush(QBrush(color));
		pen.setWidth(2);
		histo->setPen(pen);
		break;
	}
	case ColumnType::COLUMN_CURVE:
	{
		QwtPlotCurve *curve = (QwtPlotCurve*)m_qhpPlotItems[setName];
		curve->setBrush(QBrush());
		pen.setBrush(QBrush(color));
		pen.setWidth(2);
		curve->setPen(pen);
		break;
	}
	case ColumnType::COLUMN_FILLED_CURVE:
	{
		QwtPlotCurve *curve = (QwtPlotCurve*)m_qhpPlotItems[setName];
		curve->setBrush(QBrush(color));
		pen.setWidth(2);
		curve->setPen(pen);
		break;
	}
	default:
		break;
	}

	m_qhcHistoPlotColor[setName] = color;

	if (m_pDataSetBox && m_pDataSetBox->findText(setName) != -1)
	{
		m_pDataSetBox->setItemData(m_pDataSetBox->findText(setName), color, Qt::DecorationRole);
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
	if (!m_qhpPlotItems.contains(setName))
	{
		addDataSet(setName);
	}

	switch (m_ePlotColumnType)
	{
	case ColumnType::COLUMN_OUTLINE:
	case ColumnType::COLUMN_FLAT_COLUMN:
	case ColumnType::COLUMN_RAISED_COLUMN:
	case ColumnType::COLUMN_LINES:
		((QwtPlotHistogram*)m_qhpPlotItems[setName])->setSamples(new QwtIntervalSeriesData(qvSamples));
		break;
	case ColumnType::COLUMN_CURVE:
	case ColumnType::COLUMN_FILLED_CURVE:
	default:
		qDebug() << "Undefined PlotColumnType";
		break;
	}
}

//! Accepts the data for the indicated dataset for Point plots.
void BarBasePlotHandler::setPoints(const QString &setName, const QVector<QPointF> &qvPoints)
{
	if (!m_qhpPlotItems.contains(setName))
	{
		addDataSet(setName);
	}

	switch (m_ePlotColumnType)
	{
	case ColumnType::COLUMN_CURVE:
	case ColumnType::COLUMN_FILLED_CURVE:
	{
		((QwtPlotCurve *)m_qhpPlotItems[setName])->setData(new QwtPointSeriesData(qvPoints));
		break;
	}
	case ColumnType::COLUMN_OUTLINE:
	case ColumnType::COLUMN_FLAT_COLUMN:
	case ColumnType::COLUMN_RAISED_COLUMN:
	case ColumnType::COLUMN_LINES:
	default:
		qDebug() << "Undefined PlotColumnType";
		break;
	}
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
	if (!m_qhpPlotItems.contains(setName))
	{
		switch (m_ePlotColumnType)
		{
		case ColumnType::COLUMN_OUTLINE:
		case ColumnType::COLUMN_FLAT_COLUMN:
		case ColumnType::COLUMN_RAISED_COLUMN:
		case ColumnType::COLUMN_LINES:
			m_qhpPlotItems[setName] = new QwtPlotHistogram(setName);
			break;
		case ColumnType::COLUMN_CURVE:
		case ColumnType::COLUMN_FILLED_CURVE:
		{
			QwtPlotCurve *newPlotCurve = new QwtPlotCurve(setName);
			m_qhpPlotItems[setName] = newPlotCurve;
			newPlotCurve->setData(new QwtPointSeriesData(QVector<QPointF>()));
		}
			break;
		default:
			break;
		}
		m_qhpPlotItems[setName]->attach(m_pPlot);

		iLastZ = iLastZ + 2;
		m_qhpPlotItems[setName]->setZ(iLastZ);
		m_qhbHistoPlotVisibility[setName] = true;

		createErrorBars(setName);

		if (m_pDataSetBox && m_pDataSetBox->findText(setName) == -1)
		{
			m_pDataSetBox->addItem(setName);
		}
	}

	if (m_qhpPlotItems.count() > 1)
	{
		hideDataSelectionWidget(false);
	}
}

//! Sets the visibility of the indicated dataset
void BarBasePlotHandler::setVisible(const QString &setName, bool visible)
{
	if (m_qhbHistoPlotVisibility.contains(setName))
	{
		m_qhpPlotItems[setName]->setVisible(visible);
		m_qhpPlotItems[setName]->setItemAttribute(QwtPlotItem::Legend, visible);
		m_pPlot->updateLegend();
		m_qhbHistoPlotVisibility[setName] = visible;

		if (visible)
		{
			iLastZ = iLastZ + 2;
			m_qhpPlotItems[setName]->setZ(iLastZ);
		}

		setErrorBarsVisible(true);

		m_pPlot->replot();
	}
}

//! Slot triggered when the Export button is clicked.
void BarBasePlotHandler::exportClicked()
{
	QString selectedFilter;

	QString fileName = QFileDialog::getSaveFileName(
		m_pDataSelectionWidget,
		"Export Plot",
		m_tmpTitle,
		"Small (*.png *.pdf *.bmp);;Medium (*.png *.pdf *.bmp);;Large (*.png *.pdf *.bmp);;Huge (*.png *.pdf *.bmp)",
		&selectedFilter);

	//Return if canceled
	if (fileName.isEmpty())
	{
		return;
	}

	ExportType::ExportType exportType = ExportType::EXPORT_PNG;
	DataViewSize::ViewSize exportSize = DataViewSize::VIEW_SIZE_2x2;

	if (selectedFilter == "Small (*.png *.pdf *.bmp)")
	{
		exportSize = DataViewSize::VIEW_SIZE_1x1;
	}
	else if (selectedFilter == "Medium (*.png *.pdf *.bmp)")
	{
		exportSize = DataViewSize::VIEW_SIZE_2x2;
	}
	else if (selectedFilter == "Large (*.png *.pdf *.bmp)")
	{
		exportSize = DataViewSize::VIEW_SIZE_3x3;
	}
	else if (selectedFilter == "Huge (*.png *.pdf *.bmp)")
	{
		exportSize = DataViewSize::VIEW_SIZE_4x4;
	}

	if (fileName.endsWith(".png", Qt::CaseInsensitive))
	{
		exportType = ExportType::EXPORT_PNG;
	}
	else if (fileName.endsWith(".bmp", Qt::CaseInsensitive))
	{
		exportType = ExportType::EXPORT_BMP;
	}
	else if (fileName.endsWith(".pdf", Qt::CaseInsensitive))
	{
		exportType = ExportType::EXPORT_PDF;
	}
	else
	{
		exportType = ExportType::EXPORT_PNG;
		fileName += ".png";
		qDebug() << "Unrecognized export extension: defaulting to png";
	}

	exportPlot(exportType, exportSize, fileName);
}
