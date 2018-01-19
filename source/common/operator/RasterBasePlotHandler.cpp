#include <QDebug>
#include <QApplication>
#include <QThread>
#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>


#include "RasterBasePlotHandler.h"
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
#include <qwt_symbol.h>


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

	//! Constructs a new RasterBasePlotHandler
	RasterBasePlotHandler::RasterBasePlotHandler()
		: iLastZ(1), m_pDataSelectionWidget(nullptr), rowsNb_(50)
	{

	}
	RasterBasePlotHandler::~RasterBasePlotHandler()
	{
		deleteMarkers();

	}
	//! Creates or returns a widget meant to adjust dataset visibility
	QWidget *RasterBasePlotHandler::plotOptionsWidget()
	{
		if (!m_pDataSelectionWidget)
		{
			m_pDataSelectionWidget = new QWidget();
			QHBoxLayout *layout = new QHBoxLayout(m_pDataSelectionWidget);
			m_pDataSelectionWidget->setLayout(layout);

			m_pExportButton = new QPushButton("Export", m_pDataSelectionWidget);
			layout->addWidget(m_pExportButton, 1);

			QWidget* maxRowsDisplayed = new QWidget();
			QLabel *lblName = new QLabel("Trials per Plot:");
			lineEdit_ = new QLineEdit("50");
			QHBoxLayout *wlayout = new QHBoxLayout;
			wlayout->addWidget(lblName);
			wlayout->addWidget(lineEdit_);
			maxRowsDisplayed->setLayout(wlayout);

			layout->addWidget(maxRowsDisplayed, 2);
			
			layout->addStretch(3);

			connect(m_pExportButton, SIGNAL(clicked()), this, SLOT(exportClicked()));
			connect(lineEdit_, SIGNAL(textChanged(const QString &)), this, SLOT(rowsClicked(const QString &)));
		}

		return m_pDataSelectionWidget;
	}

	//! Initializes the histogram item
	void RasterBasePlotHandler::initializePlot(bool bDisplayLegend, const QColor &barColor, const QColor &canvasColor)
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
		
		m_qhcPlotColor["Default"] = barColor;

		m_pPlot->canvas()->setPalette(canvasColor);
	}


	//! Updates the rendering properties of the columns for the indicated dataset
	void RasterBasePlotHandler::updateProperties(const QString &setName, const QColor &color)
	{
		if (!m_qhpPlotItems.contains(setName))
		{
			addDataSet(setName);
		}

		updateColor(setName, color);
	}

	//! Virtual function to update the color of the named dataset
	void RasterBasePlotHandler::updateColor(const QString &setName, const QColor &color)
	{
		QPen pen(Qt::black, 0);
		
		QwtPlotCurve *curve = (QwtPlotCurve*)m_qhpPlotItems[setName];
		curve->setBrush(QBrush());
		pen.setBrush(QBrush(color));
		curve->setOrientation(Qt::Vertical);
		curve->setStyle(QwtPlotCurve::Sticks);
		pen.setWidth(2);
		curve->setPen(pen);		

		m_qhcPlotColor[setName] = color;	
	}
		
	//! Accepts the data for the indicated dataset for Point plots.
	void RasterBasePlotHandler::setPoints(const QString &setName, const QVector<QPointF> &qvPoints)
	{
		if (!m_qhpPlotItems.contains(setName))
		{
			addDataSet(setName);
		}
	
		((QwtPlotCurve *)m_qhpPlotItems[setName])->setData(new QwtPointSeriesData(qvPoints));
	}


	//! Set the scaling on the X axis
	void RasterBasePlotHandler::scaleAxis(double dBinSize)
	{
		//m_pPlot->axisScaleEngine(QwtPlot::xBottom)->setMargins(0.5*dBinSize, 0.5*dBinSize);
		m_pPlot->axisScaleEngine(QwtPlot::xBottom)->setMargins(0.01, 0.01);
	}

	//! A slot for receiving replot commands
	void RasterBasePlotHandler::callReplot(const QString &setName)
	{
		// Only replot if the submitted setname is visible
		if (m_qhbPlotVisibility.contains(setName) && m_qhbPlotVisibility[setName])
		{
			m_pPlot->replot();
		}
	}
	//! Adds the indicated dataset name to the list of datasets, and spawns a histogram item for it
	void RasterBasePlotHandler::addDataSet(const QString &setName)
	{
		if (!m_qhpPlotItems.contains(setName))
		{			
			QwtPlotCurve *newPlotCurve = new QwtPlotCurve(setName);
			m_qhpPlotItems[setName] = newPlotCurve;
			newPlotCurve->setData(new QwtPointSeriesData(QVector<QPointF>()));
			
			m_qhpPlotItems[setName]->attach(m_pPlot);

			iLastZ = iLastZ + 2;
			m_qhpPlotItems[setName]->setZ(iLastZ);
			m_qhbPlotVisibility[setName] = true;
		}

	}

	//! Slot triggered when the Export button is clicked.
	void RasterBasePlotHandler::exportClicked()
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

	void RasterBasePlotHandler::rowsClicked(const QString &newNbOfRows)
	{
		rowsNb_ = newNbOfRows.toInt();
		if (rowsNb_ > 200)
			lineEdit_->setText("200");
		else if (rowsNb_ < 0)
			lineEdit_->setText("0");
	}

	int RasterBasePlotHandler::rowsToDisplay()
	{
		return rowsNb_;
	}
	//! Clears stored plot data
	void RasterBasePlotHandler::resetSlot()
	{
		foreach(QwtPlotItem *plotItem, m_qhpPlotItems)
		{			
			((QwtPlotCurve *)plotItem)->setData(new QwtPointSeriesData());
		}
		deleteMarkers();
	}
	void RasterBasePlotHandler::addMarkers(const QString &setName, const QVector<QPointF> &qvPoints)
	{		
		if (m_pPlot)
		{
			//deleteMarkers();
			foreach(QPointF point, qvPoints)
			{
				//Add a mark to the spike plot
				QwtPlotMarker* newMark = new QwtPlotMarker();
				newMark->setLineStyle(QwtPlotMarker::NoLine);
				newMark->setLinePen(QPen(Qt::red));
				newMark->setXValue(point.x());

				//getBaseline
				if (m_qhpPlotItems.contains(setName))
				{
					QwtPlotCurve *curve = (QwtPlotCurve*)m_qhpPlotItems[setName];
					if (curve)
					{
						double baseline = curve->baseline();
						newMark->setYValue(baseline);
					}					
				}
				
				QwtSymbol *sym = new QwtSymbol(QwtSymbol::Diamond, QBrush(Qt::red), QPen(Qt::red), QSize(5, 5));				
				newMark->setSymbol(sym);

				QwtText makerLabel(QString::number(point.y()));
				makerLabel.setColor(QColor(Qt::yellow));
				newMark->setLabel(makerLabel);
				newMark->setLabelOrientation(Qt::Vertical);
				newMark->setLabelAlignment(Qt::AlignRight);
				newMark->attach(m_pPlot);
				spikeStateChangeMarkers_.append(newMark);
			}
		}		
	}
	void RasterBasePlotHandler::deleteMarkers()
	{
		if (spikeStateChangeMarkers_.size() > 0)
		{
			foreach(QwtPlotMarker* stateMarker, spikeStateChangeMarkers_)
			{
				stateMarker->detach();
				delete stateMarker;
			}
			spikeStateChangeMarkers_.clear();
		}
	}
};

