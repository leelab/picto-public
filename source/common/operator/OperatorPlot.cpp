#include "OperatorPlot.h"

#include "../memleakdetect.h"

#include <qwt_text.h>
#include <qwt_plot.h>
#include <qwt_plot_item.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_histogram.h>
#include <qwt_plot_intervalcurve.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_scale_draw.h>
#include <qwt_interval_symbol.h>
//#include <qwt_interval_series_data.h>

namespace Picto {

const QString OperatorPlot::type = "Operator Plot";

OperatorPlot::OperatorPlot()
	: m_bDataChanged(false), m_bBinsChanged(false), m_lPlotMin(0L), m_lPlotMax(1L),
	m_pPlot(nullptr)
{
	m_pPlot = new QwtPlot(QwtText("Operator Plot Test"));

	m_pPlot->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

	QwtPlotCanvas *canvas = new QwtPlotCanvas();
	canvas->setPalette(Qt::gray);
	canvas->setBorderRadius(10);
	m_pPlot->setCanvas(canvas);

	m_pPlot->plotLayout()->setAlignCanvasToScales(true);

	m_pHistoPlotItem = new QwtPlotHistogram("Data");
	m_pHistoPlotItem->setStyle(QwtPlotHistogram::Columns);
	m_pHistoPlotItem->setBrush(QBrush(Qt::red));
	m_pHistoPlotItem->attach(m_pPlot);

	QwtIntervalSymbol *errorBar = new QwtIntervalSymbol(QwtIntervalSymbol::Bar);
	errorBar->setWidth(8);
	errorBar->setPen(QColor(Qt::blue));
	
	m_pErrorBars = new QwtPlotIntervalCurve("Standard Deviation");
	m_pErrorBars->setStyle(QwtPlotIntervalCurve::NoCurve);
	m_pErrorBars->setSymbol(errorBar);
	m_pErrorBars->setRenderHint(QwtPlotItem::RenderAntialiased, false);
	m_pErrorBars->setPen(Qt::white);
	m_pErrorBars->setBrush(QBrush(QColor(Qt::blue)));

	m_pErrorBars->attach(m_pPlot);

	AddDefinableProperty(QVariant::Bool, "DisplayStdDev", false);
	m_bLastErrorBarState = false;

	AddDefinableProperty(QVariant::Bool, "NormalizedDisplay", false);
	AddDefinableProperty(QVariant::Bool, "DisplayLegend", false);
}

QSharedPointer<Asset> OperatorPlot::Create()
{
	return QSharedPointer<Asset>(new OperatorPlot());
}

void OperatorPlot::draw()
{
	if (m_bLastErrorBarState != propertyContainer_->getPropertyValue("DisplayStdDev").toBool())
	{
		m_bLastErrorBarState = !m_bLastErrorBarState;
		m_bDataChanged = true;
		m_pErrorBars->setVisible(m_bLastErrorBarState);
	}
	if (m_bBinsChanged && !m_qhlCumulNum.isEmpty())
	{
		m_bBinsChanged = false;
		QList<long> keyList = m_qhlCumulNum.keys();
		std::sort(keyList.begin(), keyList.end());
		m_lPlotMin = keyList.first();
		m_lPlotMax = keyList.last();
	}

	if (m_bDataChanged)
	{
		m_bDataChanged = false;
		replot();
	}
}

class NormalizedScaleDraw : public QwtScaleDraw
{
public:
	NormalizedScaleDraw(double max)
		:max_(max)
	{
		setTickLength(QwtScaleDiv::MajorTick, 6);
		setTickLength(QwtScaleDiv::MediumTick, 3);
		setTickLength(QwtScaleDiv::MinorTick, 0);

		setSpacing(15);
	}

	virtual QwtText label(double value) const
	{
		return QString("%1").arg(value/max_);
	}
private:
	double max_;
};

void OperatorPlot::replot()
{
	QVector<QwtIntervalSample> qvSamples;
	QVector<QwtIntervalSample> qvError;

	QList<long> keyList = m_qhlCumulNum.keys();
	std::sort(keyList.begin(), keyList.end());

	bool bNormalized = propertyContainer_->getPropertyValue("NormalizedDisplay").toBool();

	double total = 0.0;
	
	foreach(long key, keyList)
	{
		double dMin = key - 0.5;
		double dMax = key + 0.5;
		double dAvg = m_qhdCumulValue[key] / double(m_qhlCumulNum[key]);
		qvSamples.push_back(std::move(QwtIntervalSample(dAvg, dMin, dMax)));

		if (bNormalized)
		{
			total += dAvg * (dMax - dMin);
		}

		if (m_bLastErrorBarState)
		{
			double dValue = key;
			double stdDev = sqrt((m_qhdCumulValSq[key] / m_qhlCumulNum[key]) - dAvg*dAvg);
			dMin = dAvg - stdDev;
			dMax = dAvg + stdDev;
			qvError.push_back(std::move(QwtIntervalSample(dValue, dMin, dMax)));
		}
	}

	m_pHistoPlotItem->setSamples(new QwtIntervalSeriesData(qvSamples));

	if (bNormalized)
	{
		//Change the y-axis labeling to give the appearance of normalization.
		QList<double> majorTicks;
		QList<double> mediumTicks;

		int numOfDivs = 2;
		for (int i = 0; i < numOfDivs+1; i++)
		{
			majorTicks += (i/(1.0*numOfDivs))*total;
		}
		
		numOfDivs = 5;
		for (int i = 0; i < numOfDivs + 1; i++)
		{
			mediumTicks += (i / (1.0*numOfDivs))*total;
		}

		QList<double> minorTicks;

		QwtScaleDiv scaleDiv(0, total + 1, minorTicks, mediumTicks, majorTicks);

		QwtScaleDraw *scaleDraw = new NormalizedScaleDraw(total);

		m_pPlot->setAxisScaleDiv(QwtPlot::yLeft, scaleDiv);
		m_pPlot->setAxisScaleDraw(QwtPlot::yLeft, scaleDraw);
	}

	m_pErrorBars->setSamples(new QwtIntervalSeriesData(qvError));

	m_pPlot->replot();

	//QPixmap pixmap(m_pPlot->width(), m_pPlot->height());
	//pixmap.grabWidget(m_pPlot);
	//image_ = QImage(pixmap);
}


/*! \brief Registers plot with the Task object, so the task can pass on the information to various UI elements, and
 *	intercept/handle scripting language commands.
 */
void OperatorPlot::postDeserialize()
{
	DataViewElement::postDeserialize();

	getTaskConfig()->addObserverWidget(this, m_pPlot);

	setPropertyRuntimeEditable("DisplayStdDev");

	m_bLastErrorBarState = propertyContainer_->getPropertyValue("DisplayStdDev").toBool();
	propertyContainer_->getProperty("DisplayStdDev")->enableInitRunValueSync(true);
	
	if (propertyContainer_->getPropertyValue("DisplayLegend").toBool())
	{
		QwtLegend *legend = new QwtLegend;
		legend->setDefaultItemMode(QwtLegendData::ReadOnly);
		m_pPlot->insertLegend(legend, QwtPlot::RightLegend);
	}
	else
	{
		m_pPlot->insertLegend(nullptr);
	}
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

void OperatorPlot::setBin(long bin, double value)
{
	m_bDataChanged = true;

	if (m_qhdCumulValue.contains(bin))
	{
		m_qhdCumulValue[bin] = value;
	}
	else
	{
		m_bBinsChanged = true;

		m_qhlCumulNum[bin] = 1;
		m_qhdCumulValue[bin] = value;
		m_qhdCumulValSq[bin] = value*value;
	}
}

void OperatorPlot::setBinSquared(long bin, double value)
{
	m_bDataChanged = true;

	if (m_qhdCumulValSq.contains(bin))
	{
		m_qhdCumulValSq[bin] = value;
	}
	else
	{
		m_bBinsChanged = true;

		m_qhlCumulNum[bin] = 1;
		m_qhdCumulValue[bin] = sqrt(value);
		m_qhdCumulValSq[bin] = value;
	}
}

void OperatorPlot::setBinSamples(long bin, long samples)
{
	m_bDataChanged = true;

	if (m_qhlCumulNum.contains(bin))
	{
		m_qhlCumulNum[bin] = samples;
	}
	else
	{
		m_bBinsChanged = true;

		m_qhlCumulNum[bin] = samples;
		m_qhdCumulValue[bin] = 0;
		m_qhdCumulValSq[bin] = 0;
	}
}

double OperatorPlot::getBin(long bin)
{
	if (m_qhdCumulValue.contains(bin))
	{
		return m_qhdCumulValue[bin];
	}

	return 0.0;
}

double OperatorPlot::getBinSquared(long bin)
{
	if (m_qhdCumulValSq.contains(bin))
	{
		return m_qhdCumulValSq[bin];
	}

	return 0.0;
}

long OperatorPlot::getSamples(long bin)
{
	if (m_qhlCumulNum.contains(bin))
	{
		return m_qhlCumulNum[bin];
	}

	return 0L;
}

void OperatorPlot::reset()
{
	m_qhdCumulValue.clear();
	m_qhdCumulValSq.clear();
	m_qhlCumulNum.clear();

	m_bDataChanged = true;
	m_bBinsChanged = true;
}


bool OperatorPlot::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if (!DataViewElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

void OperatorPlot::setTitle(const QString &newTitle)
{
	if (m_pPlot)
	{
		m_pPlot->setTitle(QwtText(newTitle));
	}
}

const QString OperatorPlot::getTitle() const
{
	if (m_pPlot)
	{
		return m_pPlot->title().text();
	}

	return QString("");
}

}; //namespace Picto
