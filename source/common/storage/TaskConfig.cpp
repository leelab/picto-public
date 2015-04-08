#include <QtDebug>

#include "TaskConfig.h"
#include "../operator/DataViewElement.h"

#include "../memleakdetect.h"

namespace Picto
{

TaskConfig::TaskConfig()
	:updateSignalEnabled_(false)
{
	reset();
}

/*! \brief Clears out all of this object's saved data.  Returning it to its initial state.
 */
void TaskConfig::reset()
{
	widgetMap.clear();
	widgetNameMap.clear();

	updateSignalEnabled_ = false;
}

/*! \brief Adds the passed in ObserverWidget to the TaskConfig.
 */
void TaskConfig::addObserverWidget(DataViewElement *owningAsset, QWidget *widget)
{
	if (!widgetMap.contains(owningAsset))
	{
		widgetMap[owningAsset] = widget;
	}
	else
	{
		qDebug() << "Same Asset added twice to TaskConfig";
	}

	widgetNameMap[widget] = owningAsset->getTitle();

	if (updateSignalEnabled_)
	{
		emit widgetAddedToMap(widget);
	}
}

/*! \brief Updates the name stored for the referenced Widget.
 */
void TaskConfig::updateWidgetName(QWidget *widget, const QString &newName)
{
	if (widgetNameMap.contains(widget))
	{
		widgetNameMap[widget] = newName;
	}
	else
	{
		qDebug() << "Widget Title modified for non-existant Widget";
	}

	if (updateSignalEnabled_)
	{
		emit widgetMapUpdated(widget);
	}
}

/*! \brief Removes the passed widget from the widgetNameMap, as well all assets pointing to it from the widgetMap.
 */
void TaskConfig::removeWidget(QWidget *widget)
{
	if (widgetNameMap.contains(widget))
	{
		if (updateSignalEnabled_)
		{
			emit widgetRemovedFromMap(widget);
		}

		//Remove the naem from the name map
		widgetNameMap.remove(widget);

		//Iterate over widgetsMap and find elements that point to the widget we want to remove
		for (auto it = widgetMap.begin(); it != widgetMap.end();)
		{
			if (it.value() == widget)
			{
				widgetMap.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

/*! \brief Removes the passed asset from the widgetMap, as well as its associated widget from the widgetNameMap.
 */
void TaskConfig::removeAsset(DataViewElement *owningAsset)
{
	if (widgetMap.contains(owningAsset))
	{
		if (updateSignalEnabled_)
		{
			emit widgetRemovedFromMap(widgetMap[owningAsset]);
		}

		widgetNameMap.remove(widgetMap[owningAsset]);
		widgetMap.remove(owningAsset);

	}
}

/*! \brief Toggles whether widgetMapUpdated() is emitted every time a change occurs.
 */
void TaskConfig::setUpdateSignalEnabled(bool bEnabled)
{
	updateSignalEnabled_ = bEnabled;
}


/*! \brief Returns a QList of the current View Widgets.
*/
const QList<QWidget *> TaskConfig::getWidgets() const
{
	return widgetNameMap.keys();
}

/*! \brief Returns a QList of the names of the current View Widgets.
*/
const QList<QString> TaskConfig::getNames() const
{
	return widgetNameMap.values();
}

const QString TaskConfig::getName(QWidget *pWidget) const
{
	if (widgetNameMap.contains(pWidget))
	{
		return widgetNameMap[pWidget];
	}

	return QString("");
}

DataViewElement *TaskConfig::getAsset(QWidget *pWidget) const
{
	return widgetMap.key(pWidget);
}

}
