#include <QDebug>
#include <QApplication>
#include <QThread>
#include <QVBoxLayout>

#include "../storage/TaskConfig.h"
#include "OperatorPlotHandler.h"

#include "../memleakdetect.h"

#include <qwt_plot_histogram.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_renderer.h>

using namespace Picto;


OperatorPlotHandler::OperatorPlotHandler()
	: m_pDataViewWidget(nullptr), m_pPlot(nullptr), m_tmpTitle("")
{
	static int count = 0;
	index = count++;
}

OperatorPlotHandler::~OperatorPlotHandler()
{
	if (m_pPlot && !m_pPlot->parent())
	{
		m_pPlot->deleteLater();
		m_pPlot = nullptr;
	}
	else if (m_pDataViewWidget && !m_pDataViewWidget->parent())
	{
		m_pDataViewWidget->deleteLater();
		m_pDataViewWidget = nullptr;
		m_pPlot = nullptr;
	}
}

void OperatorPlotHandler::initializePlot(const QString &xTitle, const QString &yTitle)
{
	if (!m_pPlot)
	{
		m_pPlot = new QwtPlot(QwtText(m_tmpTitle));

		m_pPlot->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	}


	QwtPlotCanvas *pCanvas = new QwtPlotCanvas();
	pCanvas->setPalette(Qt::gray);
	pCanvas->setBorderRadius(10);
	m_pPlot->setCanvas(pCanvas);

	m_pPlot->setAxisTitle(QwtPlot::xBottom, xTitle);
	m_pPlot->setAxisTitle(QwtPlot::yLeft, yTitle);
}

void OperatorPlotHandler::connectToTaskConfig(QSharedPointer<TaskConfig> pTaskConfig)
{
	DataViewElement *pSender = (DataViewElement*)QObject::sender();

	if (dataSelectionWidget())
	{
		m_pDataViewWidget = new QWidget();
		QVBoxLayout *layout = new QVBoxLayout(m_pDataViewWidget);
		m_pDataViewWidget->setLayout(layout);
		layout->addWidget(dataSelectionWidget(), 0);
		layout->setContentsMargins(0, 0, 0, 0);
		dataSelectionWidget()->setVisible(false);
		layout->addWidget(m_pPlot, 1);
	}
	else
	{
		m_pDataViewWidget = m_pPlot;
	}

	pTaskConfig->addObserverWidget(pSender, m_pDataViewWidget);
}

void OperatorPlotHandler::setTitle(const QString &title)
{
	if (m_pPlot)
	{
		m_pPlot->setTitle(QwtText(title));
		m_tmpTitle = title;
	}
	else
	{
		m_tmpTitle = title;
	}
}


void OperatorPlotHandler::exportPlot(int type, int size, const QString fileName)
{
	if (m_pPlot)
	{
		ExportType::ExportType exportType = (ExportType::ExportType) type;
		QString typeName = "";
		switch (exportType)
		{
		case ExportType::EXPORT_PDF:
			typeName = "pdf";
			break;
		case ExportType::EXPORT_PNG:
			typeName = "png";
			break;
		case ExportType::EXPORT_BMP:
			typeName = "bmp";
			break;
		case ExportType::EXPORT_POSTSCRIPT:
			typeName = "ps";
			break;
		default:
			qDebug() << "Unexpected Export Type";
			return;
		}

		DataViewSize::ViewSize exportSize = (DataViewSize::ViewSize) size;
		QSizeF renderSize;

		switch (exportSize)
		{
		case DataViewSize::VIEW_SIZE_1x1:
			renderSize = QSizeF(80, 60);
			break;
		case DataViewSize::VIEW_SIZE_2x2:
			renderSize = QSizeF(160, 120);
			break;
		case DataViewSize::VIEW_SIZE_3x3:
			renderSize = QSizeF(240, 180);
			break;
		case DataViewSize::VIEW_SIZE_4x4:
			renderSize = QSizeF(320, 240);
			break;
		default:
			renderSize = m_pPlot->size() * 0.264583333;
			break;
		}
		

		QwtPlotRenderer renderer;
		renderer.renderDocument(m_pPlot, fileName, typeName, renderSize);
	}
}

void OperatorPlotHandler::requestExport(ExportType::ExportType type)
{
	if (m_pPlot)
	{
		QString filename = m_tmpTitle;
		switch (type)
		{
		case ExportType::EXPORT_PDF:
			filename += ".pdf";
			break;
		case ExportType::EXPORT_PNG:
			filename += ".png";
			break;
		case ExportType::EXPORT_BMP:
			filename += ".bmp";
			break;
		case ExportType::EXPORT_POSTSCRIPT:
			filename += ".ps";
			break;
		default:
			qDebug() << "Unexpected Export Type";
			return;
		}


		QwtPlotRenderer renderer;
		renderer.exportTo(m_pPlot, filename);
	}
}