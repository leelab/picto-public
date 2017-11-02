#include <QDebug>
#include <QApplication>

#include "SamplingBarBasePlotHandler.h"
#include "DataViewSpecs.h"

#include "../memleakdetect.h"

#include <qwt_interval_symbol.h>
#include <qwt_plot_intervalcurve.h>
#include <qwt_plot_histogram.h>

using namespace Picto;


namespace Picto {

SamplingBarBasePlotHandler::SamplingBarBasePlotHandler()
{

}

void SamplingBarBasePlotHandler::initializeSampling()
{

}

void SamplingBarBasePlotHandler::setErrorSamples(const QString &setName, const QVector<QwtIntervalSample> &qvErrorSamples)
{
	if (m_qhpErrorBars.contains(setName))
		m_qhpErrorBars[setName]->setSamples(new QwtIntervalSeriesData(qvErrorSamples));
}

void SamplingBarBasePlotHandler::setErrorBarsVisible(bool bVisible)
{
	//Set the visibility of each errorbar to be equal to that of its corresponding plot
	foreach(QString setName, m_qhpErrorBars.keys())
	{
		m_qhpErrorBars[setName]->setVisible(bVisible && m_qhbHistoPlotVisibility[setName]);
		m_qhpErrorBars[setName]->setZ(m_qhpPlotItems[setName]->z() + 1);
	}
}

void SamplingBarBasePlotHandler::createErrorBars(const QString &dataSet)
{
	QwtIntervalSymbol *errorBar = new QwtIntervalSymbol(QwtIntervalSymbol::Bar);
	errorBar->setWidth(8);

	m_qhpErrorBars[dataSet] = new QwtPlotIntervalCurve(dataSet + " StdErr");
	m_qhpErrorBars[dataSet]->setStyle(QwtPlotIntervalCurve::NoCurve);
	m_qhpErrorBars[dataSet]->setSymbol(errorBar);
	m_qhpErrorBars[dataSet]->setRenderHint(QwtPlotItem::RenderAntialiased, false);
	m_qhpErrorBars[dataSet]->setPen(Qt::white);
	m_qhpErrorBars[dataSet]->setItemAttribute(QwtPlotItem::Legend, false);
	

	if (m_qhcErrorBarColor.contains(dataSet))
	{
		errorBar->setPen(m_qhcErrorBarColor[dataSet]);
		m_qhpErrorBars[dataSet]->setBrush(QBrush(m_qhcErrorBarColor[dataSet]));
	}
	else
	{
		errorBar->setPen(QColor(Qt::blue));
		m_qhpErrorBars[dataSet]->setBrush(QBrush(QColor(Qt::blue)));
	}

	m_qhpErrorBars[dataSet]->attach(m_pPlot);
	m_qhpErrorBars[dataSet]->setZ(m_qhpPlotItems[dataSet]->z() + 1);
}

void SamplingBarBasePlotHandler::resetSlot()
{
	BarBasePlotHandler::resetSlot();

	foreach(QwtPlotIntervalCurve *errorBar, m_qhpErrorBars)
	{
		errorBar->setData(new QwtIntervalSeriesData());
	}
	
}

void SamplingBarBasePlotHandler::clearPlotItems()
{
	BarBasePlotHandler::clearPlotItems();
	
	QHash<QString, QwtPlotIntervalCurve*>::iterator iter = m_qhpErrorBars.begin();
	while (iter != m_qhpErrorBars.end())
	{
		(*iter)->detach();
		delete *iter;
		iter = m_qhpErrorBars.erase(iter);
	}

	m_qhcErrorBarColor.clear();
}

void SamplingBarBasePlotHandler::updateColor(const QString &setName, const QColor &color)
{
	BarBasePlotHandler::updateColor(setName, color);

	QColor newColor;

	switch (m_ePlotColumnType)
	{
	case ColumnType::COLUMN_FLAT_COLUMN:
	case ColumnType::COLUMN_RAISED_COLUMN:
	case ColumnType::COLUMN_FILLED_CURVE:
	{
		newColor = color.toHsv();

		if (newColor.value() >= 128)
		{
			newColor.setHsv(newColor.hslHue(), newColor.hslSaturation(), newColor.value() - 100);
		}
		else
		{
			newColor.setHsv(newColor.hslHue(), newColor.hslSaturation(), newColor.value() + 100);
		}

		m_qhpErrorBars[setName]->setBrush(QBrush(newColor));
		if (m_qhpErrorBars[setName]->symbol())
		{
			((QwtIntervalSymbol *)m_qhpErrorBars[setName]->symbol())->setPen(newColor);
		}
	}
		break;
	case ColumnType::COLUMN_OUTLINE:
	case ColumnType::COLUMN_LINES:
	case ColumnType::COLUMN_CURVE:
		newColor = color;
		m_qhpErrorBars[setName]->setBrush(QBrush(newColor));
		if (m_qhpErrorBars[setName]->symbol())
		{
			((QwtIntervalSymbol *)m_qhpErrorBars[setName]->symbol())->setPen(newColor);
		}
		break;
	default:
		break;
	}

	m_qhcErrorBarColor[setName] = std::move(newColor);
}

}; //namespace Picto