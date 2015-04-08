#ifndef _TASK_CONFIG_H_
#define _TASK_CONFIG_H_

#include <QObject>
#include <QMap>
#include <QList>

#include "../common.h"

namespace Picto {
class DataViewElement;
/*! \brief There are many operations and data elements that need to be associated with a task as a whole, accessible by
 *	all elements of the task and with access to all elements of the task.  Objects of the TaskConfig class handle
 *	these types of operations and store this type of data.  This is a lower-scoped analog to the DesignConfig class, and
 *	much lighter-weight.
 *	
 *	\details Some of the types of operations that require access to/from all task Assets are: 
 *	- Configuring and accessing Task plot objects
 *	- (FUTURE) Uniquely gating Input vs Output on the NIDaq Digital lines
 *
 *	These and other general task configuration issues are handled by this class.  Every Picto Asset holds a pointer to
 *	the TaskConfig of the task to which it is attached and elements needing to be accessed through the TaskConfig add
 *	themselves to the TaskConfig object (which holds a weak pointer to them to avoid circular shared pointer issues).
 *	The TaskConfig of a particular Task can be accessed using its Asset::getTaskConfig() function.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API TaskConfig : public QObject
#else
class TaskConfig : public QObject
#endif
{
	Q_OBJECT
public:
	TaskConfig();
	virtual ~TaskConfig(){};

	void addObserverWidget(DataViewElement *owningAsset, QWidget *widget);
	void updateWidgetName(QWidget *widget, const QString &newName);

	void removeWidget(QWidget *widget);
	void removeAsset(DataViewElement *owningAsset);

	void reset();

	void setUpdateSignalEnabled(bool bEnabled = true);

	const QList<QWidget *> getWidgets() const;
	const QList<QString> getNames() const;

	const QString getName(QWidget *pWidget) const;
	DataViewElement *getAsset(QWidget *pWidget) const;

signals:
	/*! \brief A signal sent whenever the title or other associated values of a viewer widget is updated.
	 */
	void widgetMapUpdated(QWidget *pWidget);
	/*! \brief A signal sent whenever a viewer widget is added to the Task.
	*/
	void widgetAddedToMap(QWidget *pWidget);
	/*! \brief A signal sent whenever a viewer widget is removed from the Task.
	*/
	void widgetRemovedFromMap(QWidget *pWidget);

protected:
	QMap<DataViewElement*, QWidget*> widgetMap;
	QMap<QWidget*,QString> widgetNameMap;
	bool updateSignalEnabled_;


};

}; //namespace Picto

#endif
