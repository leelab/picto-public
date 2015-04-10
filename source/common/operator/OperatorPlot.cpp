#include "OperatorPlot.h"

#include "../memleakdetect.h"

#include <qwt_text.h>
#include <qwt_plot_canvas.h>

namespace Picto {

const QString OperatorPlot::type = "Operator Plot";

OperatorPlot::OperatorPlot()
	: m_bDataChanged(false), m_pPlot(nullptr)
{
	m_pPlot = new QwtPlot(QwtText(""));

	m_pPlot->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

	QwtPlotCanvas *canvas = new QwtPlotCanvas();
	canvas->setPalette(Qt::gray);
	canvas->setBorderRadius(10);
	m_pPlot->setCanvas(canvas);
}

void OperatorPlot::draw()
{
	if (m_bDataChanged)
	{
		m_bDataChanged = false;
		replot();
	}
}


/*! \brief Registers plot with the Task object, so the task can pass on the information to various UI elements, and
 *	intercept/handle scripting language commands.
 */
void OperatorPlot::postDeserialize()
{
	DataViewElement::postDeserialize();

	getTaskConfig()->addObserverWidget(this, m_pPlot);
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
