#include <QDebug>
#include <QApplication>

#include "OperatorPlotHandler.h"

#include "../memleakdetect.h"

#include <qwt_plot_histogram.h>
#include <qwt_plot_canvas.h>

using namespace Picto;


OperatorPlotHandler::OperatorPlotHandler()
	: m_pPlot(nullptr)
{

}

OperatorPlotHandler::~OperatorPlotHandler()
{
	qDebug() << "Delete OperatorPlot";
	if (m_pPlot && !m_pPlot->parent())
	{
		qDebug() << "Manually Delete OperatorPlot::m_pPlot";
		delete m_pPlot;
	}
}

void OperatorPlotHandler::initializePlot(const QString &xTitle, const QString &yTitle)
{
	m_pPlot = new QwtPlot(QwtText(""));

	m_pPlot->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	QwtPlotCanvas *pCanvas = new QwtPlotCanvas();
	pCanvas->setPalette(Qt::gray);
	pCanvas->setBorderRadius(10);
	m_pPlot->setCanvas(pCanvas);

	m_pPlot->setAxisTitle(QwtPlot::xBottom, xTitle);
	m_pPlot->setAxisTitle(QwtPlot::yLeft, yTitle);
}

void OperatorPlotHandler::setTitle(const QString &title)
{
	m_pPlot->setTitle(QwtText(title));
}
