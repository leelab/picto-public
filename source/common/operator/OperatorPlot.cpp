#include <QApplication>
#include <QThread>

#include "OperatorPlot.h"

#include "../memleakdetect.h"

#include "OperatorPlotHandler.h"

namespace Picto {

const QString OperatorPlot::type = "Operator Plot";

OperatorPlot::OperatorPlot()
	: m_bDataChanged(false), m_title(""), initialized_(false)
{
	AddDefinableProperty(QVariant::String, "XTitle", "");
	AddDefinableProperty(QVariant::String, "YTitle", "");
}

OperatorPlot::~OperatorPlot()
{
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

	initialized_ = false;
}

void OperatorPlot::postLinkUpdate()
{
	DataViewElement::postLinkUpdate();

	m_pPlotHandler = getTaskConfig()->getPlotHandler(getPath());

	if (m_pPlotHandler.isNull())
	{
		m_pPlotHandler = getNewHandler();
		m_pPlotHandler->moveToThread(QApplication::instance()->thread());
		getTaskConfig()->setPlotHandler(getPath(), m_pPlotHandler);
	}

	connectHandler(m_pPlotHandler);
}

void OperatorPlot::setTitle(const QString &newTitle)
{
	emit setTitleSig(newTitle);
	m_title = newTitle;
}

const QString OperatorPlot::getTitle() const
{
	return m_title;
}

QSharedPointer<OperatorPlotHandler> OperatorPlot::getNewHandler()
{
	return QSharedPointer<OperatorPlotHandler>(new OperatorPlotHandler());
}

void OperatorPlot::connectHandler(QSharedPointer<OperatorPlotHandler> plotHandler)
{
	qRegisterMetaType<QSharedPointer<TaskConfig>>("QSharedPointer<TaskConfig>");
	connect(this, SIGNAL(connectToTaskConfigSig(QSharedPointer<TaskConfig>)),
		plotHandler.data(), SLOT(connectToTaskConfig(QSharedPointer<TaskConfig>)));
}

void OperatorPlot::connectDataSignals(QSharedPointer<OperatorPlotHandler> plotHandler)
{
	connect(this, SIGNAL(setTitleSig(const QString&)),
		plotHandler.data(), SLOT(setTitle(const QString&)));
	connect(this, SIGNAL(initializePlotSig(const QString&, const QString&)),
		plotHandler.data(), SLOT(initializePlot(const QString&, const QString&)));
}

void OperatorPlot::initView()
{
	qDebug() << "\tOperatorPlot::initView Operator Plot (" << dveNum_ << ") Connected to Handler.";
	connectDataSignals(m_pPlotHandler);
	DataViewElement::initView();

	emit initializePlotSig(propertyContainer_->getPropertyValue("XTitle").toString(),
		propertyContainer_->getPropertyValue("YTitle").toString());
}

void OperatorPlot::sendWidgetToTaskConfig()
{
	initView();
	emit connectToTaskConfigSig(getTaskConfig());
}

void OperatorPlot::enteredScope()
{
	reset();
}

void OperatorPlot::scriptableContainerWasReinitialized()
{
	if (!initialized_ && qsEngine_)
	{
		initialized_ = true;
		QScriptValue returnVal(false);
		runScript(getName().simplified().remove(' '), returnVal);
	}
}

}; //namespace Picto
