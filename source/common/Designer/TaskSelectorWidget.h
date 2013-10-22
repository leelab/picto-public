#ifndef TaskSelectorWidget_H
#define TaskSelectorWidget_H

#include <QWidget>
#include <QSharedPointer>
#include <QButtonGroup>
#include <QMap>
#include <QIcon>
#include "../design/DesignRoot.h"
#include "../parameter/Analysis.h"

namespace Picto {
//! [0]
//Widget for selecting from a list of available tasks
#if defined WIN32 || defined WINCE
class PICTOLIB_API TaskSelectorWidget : public QWidget
#else
class TaskSelectorWidget : public QWidget
#endif
{
    Q_OBJECT

public:
	TaskSelectorWidget(QWidget *parent = NULL);
	virtual ~TaskSelectorWidget(){};
	void addRun(QIcon& icon,QString runName,int index);
	void clear();
	int getSelectedRun();
signals:
	void runSelected(int runIndex);
private:
	QButtonGroup* buttonGroup_;
};
};
//! [0]
#endif
