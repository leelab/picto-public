#include "OperatorPlot.h"

#include "../memleakdetect.h"

#include <qwt_text.h>
#include <qwt_plot_canvas.h>

namespace Picto {

const QString OperatorPlot::type = "Operator Plot";

OperatorPlot::OperatorPlot()
	: m_bDataChanged(false), m_pPlot(nullptr), m_title("")
{
	qDebug() << "Create OperatorPlot";
	AddDefinableProperty(QVariant::String, "XTitle", "");
	AddDefinableProperty(QVariant::String, "YTitle", "");
}

OperatorPlot::~OperatorPlot()
{
	qDebug() << "Delete OperatorPlot";
	if (m_pPlot && !m_pPlot->parent())
	{
		qDebug() << "Manually Delete OperatorPlot::m_pPlot";
		delete m_pPlot;
	}
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

	qDebug() << "Create OperatorPlot::m_pPlot";
	
	m_pPlot = new QwtPlot(QwtText(""));
	m_title = "";

	m_pPlot->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	QwtPlotCanvas *pCanvas = new QwtPlotCanvas();
	pCanvas->setPalette(Qt::gray);
	pCanvas->setBorderRadius(10);
	m_pPlot->setCanvas(pCanvas);

	m_pPlot->setAxisTitle(QwtPlot::xBottom, propertyContainer_->getPropertyValue("XTitle").toString());
	m_pPlot->setAxisTitle(QwtPlot::yLeft, propertyContainer_->getPropertyValue("YTitle").toString());

	getTaskConfig()->addObserverWidget(this, m_pPlot);
}

void OperatorPlot::setTitle(const QString &newTitle)
{
	if (m_pPlot)
	{
		m_pPlot->setTitle(QwtText(newTitle));
		m_title = newTitle;
	}
}

const QString OperatorPlot::getTitle() const
{
	if (m_pPlot)
	{
		return m_title;
	}

	return QString("");
}

}; //namespace Picto
