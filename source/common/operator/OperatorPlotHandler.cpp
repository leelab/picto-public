#include <QDebug>
#include <QApplication>
#include <QThread>

#include "../storage/TaskConfig.h"
#include "OperatorPlotHandler.h"

#include "../memleakdetect.h"

#include <qwt_plot_histogram.h>
#include <qwt_plot_canvas.h>

using namespace Picto;


OperatorPlotHandler::OperatorPlotHandler()
	: m_pPlot(nullptr), m_tmpTitle("")
{
	static int count = 0;
	index = count++;

	qDebug() << "OperatorPlotHandler (" << index << ") created";
}

OperatorPlotHandler::~OperatorPlotHandler()
{
	qDebug() << "Delete OperatorPlot (" << index << ")";
	if (m_pPlot && !m_pPlot->parent())
	{
		qDebug() << "\tManually Deleted OperatorPlot::m_pPlot";
		m_pPlot->deleteLater();
	}
}

void OperatorPlotHandler::initializePlot(const QString &xTitle, const QString &yTitle)
{
	if (!m_pPlot)
	{
		qDebug() << "\tOperatorPlotHandler (" << index << ") created QwtPlot";
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
