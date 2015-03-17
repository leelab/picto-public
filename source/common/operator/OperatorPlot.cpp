#include "OperatorPlot.h"
#include "../memleakdetect.h"
#include <qwt_text.h>
#include <qwt_plot_item.h>
//#include <qwt_interval_series_data.h>

namespace Picto {

const QString OperatorPlot::type = "Operator Plot";

OperatorPlot::OperatorPlot()
	: m_bDataChanged(false), m_bBinsChanged(false), m_lPlotMin(0L), m_lPlotMax(1L),
	m_pPlot(nullptr)
{
	m_pPlot = new QwtPlot(QwtText("Operator Plot Test"));

	m_pHistoPlotItem = new QwtPlotHistogram(QwtText("Operator Plot Histo Test"));
	m_pHistoPlotItem->attach(m_pPlot);
}

QSharedPointer<Asset> OperatorPlot::Create()
{
	return QSharedPointer<Asset>(new OperatorPlot());
}

void OperatorPlot::draw()
{
	if (m_bBinsChanged)
	{
		m_bBinsChanged = false;
		QList<long> keyList = m_qhlCumulNum.keys();
		std::sort(keyList.begin(), keyList.end());
		m_lPlotMin = keyList.first();
		m_lPlotMax = keyList.last();
	}

	if (m_bDataChanged)
	{
		replot();
	}
}

void OperatorPlot::replot()
{
	QVector<QwtIntervalSample> pqvSamples;

	QList<long> keyList = m_qhlCumulNum.keys();
	std::sort(keyList.begin(), keyList.end());
	
	foreach(long key, keyList)
	{
		double dMin = key;
		double dMax = key + 1;
		double dValue = m_qhdCumulValue[key] / double(m_qhlCumulNum[key]);
		pqvSamples.push_back(std::move(QwtIntervalSample(dValue, dMin, dMax)));
	}

	m_pHistoPlotItem->setSamples(new QwtIntervalSeriesData(pqvSamples));

	m_pPlot->replot();

	//QPixmap pixmap(m_pPlot->width(), m_pPlot->height());
	//pixmap.grabWidget(m_pPlot);
	//image_ = QImage(pixmap);
}

void OperatorPlot::postDeserialize()
{
	DataViewElement::postDeserialize();
}

void OperatorPlot::submitValue(long iBin, double dValue)
{
	m_bDataChanged = true;

	if (m_qhlCumulNum.contains(iBin))
	{
		m_qhlCumulNum[iBin]++;
		m_qhdCumulValue[iBin] += dValue;
		m_qhdCumulValSq[iBin] += dValue*dValue;
	}
	else
	{
		m_bBinsChanged = true;

		m_qhlCumulNum[iBin] = 1;
		m_qhdCumulValue[iBin] = dValue;
		m_qhdCumulValSq[iBin] = dValue*dValue;
	}
}

void OperatorPlot::dropBin(long iBin)
{
	if (m_qhlCumulNum.contains(iBin))
	{
		m_bDataChanged = true;
		m_bBinsChanged = true;

		m_qhlCumulNum.remove(iBin);
		m_qhdCumulValue.remove(iBin);
		m_qhdCumulValSq.remove(iBin);
	}
}

bool OperatorPlot::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if (!DataViewElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
