#include <QDebug>
#include <QApplication>
#include <QThread>

#include "../storage/TaskConfig.h"
#include "OperatorPlotHandler.h"

#include "../memleakdetect.h"

#include <qwt_plot_histogram.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_renderer.h>

using namespace Picto;


OperatorPlotHandler::OperatorPlotHandler()
	: m_pPlot(nullptr), m_tmpTitle("")
{
	static int count = 0;
	index = count++;
}

OperatorPlotHandler::~OperatorPlotHandler()
{
	if (m_pPlot && !m_pPlot->parent())
	{
		m_pPlot->deleteLater();
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
	pTaskConfig->addObserverWidget(pSender, m_pPlot);
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


void OperatorPlotHandler::exportPlot(int type, const QString fileName)
{
	ExportType::ExportType exportType = (ExportType::ExportType) type;
	if (m_pPlot)
	{
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
		

		QwtPlotRenderer renderer;
		renderer.renderDocument(m_pPlot, fileName, typeName, m_pPlot->size());
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