#ifndef TaskSelectorWidget_H
#define TaskSelectorWidget_H

#include <QWidget>
#include <QSharedPointer>
#include <QButtonGroup>
#include <QMap>
#include <QIcon>
#include <QGroupBox>
#include <QPushButton>
#include <QColor>
#include <QScrollArea>
#include <QSharedPointer>
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
	void addRun(bool saved,QString filePath,QString runName,int index,QString notes = "");
	void clear();
	QList<int> getSelectedRuns(QString filePath);
	QStringList getSelectedFilePaths();
	int getNumSelectedRuns();
	enum RunStatus {
		IDLE,
		INPROGRESS,
		COMPLETE,
		ERROROCCURED
		};
	void setRunStatus(QString fileName,int runIndex,RunStatus status);
	int getRunStatus(QString fileName,int runIndex);
	void setRunInProgress(QString fileName,int runIndex);
	void setRunComplete(QString fileName,int runIndex);
	void setRunError(QString fileName,int runIndex);
	void resetAllRunStatus();
signals:
	void runSelectionChanged();
private:
	void setRunColor(QString fileName,int runIndex,QColor color);
	QScrollArea* scrollArea_;
	QWidget* mainWidget_;
	QButtonGroup* buttonGroup_;
	QGroupBox* currGroupBox_;
	QPushButton* selectAll_;
	QPushButton* clearSelection_;
	QPushButton* selectSaved_;
	int numRuns_;
	int numSessions_;
	struct RunInfo
	{
		RunInfo(){fileName_ = "";index_ = -1;runStatus_ = IDLE;saved_ = false;button_=NULL;};
		RunInfo(QString fileName,int index,bool saved,QAbstractButton* button,RunStatus runStatus = IDLE){fileName_ = fileName;index_ = index;saved_ = saved;runStatus_ = runStatus;button_ = button;};
		QString fileName_;
		int index_;
		RunStatus runStatus_;
		bool saved_;
		QAbstractButton* button_;
	};
	QMap<int,QSharedPointer<RunInfo>> buttonIdRunLookup_;
	QMap<QString,QMap<int,QSharedPointer<RunInfo>>> fileRunLookup_;
private slots:
	void buttonClicked(int buttonIndex);
	void selectAll();
	void selectSaved();
	void clearSelection();
};
};
//! [0]
#endif
