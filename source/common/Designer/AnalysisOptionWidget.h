#ifndef AnalysisOptionWidget_H
#define AnalysisOptionWidget_H

#include <QWidget>
#include <QSharedPointer>
#include <QComboBox>
#include <QAction>
#include "EditorState.h"
#include "../design/DesignRoot.h"
#include "../parameter/Analysis.h"

namespace Picto {
/*! \brief A drop down menu for selecting Analyses for the Designer.
 *	\details This widget is used to control Analysis editing in the Designer.  Originally it was also responsible for
 *	selecting Analysis when running an experiment, but more flexibility was required for that, so the
 *	AnalysisSelectorWidget was developed.
 *	\sa AnalysisSelectorWidget
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API AnalysisOptionWidget : public QWidget
#else
class AnalysisOptionWidget : public QWidget
#endif
{
    Q_OBJECT

public:
	AnalysisOptionWidget(QSharedPointer<EditorState> editorState, QWidget *parent = NULL);
	virtual ~AnalysisOptionWidget(){};
	void setDesignRoot(QSharedPointer<DesignRoot> designRoot);
	void setSelectedAnalysis(QUuid analysisId);
	void setSelectedAnalysis(QSharedPointer<Analysis> analysis);
	QSharedPointer<Analysis> getSelectedAnalysis();
	QUuid getSelectedAnalysisId();

	void setSelectedTask(QSharedPointer<Task> task);
	QSharedPointer<Task> getSelectedTask();
public slots:
	void updateAnalysisList();
	void updateTaskList();

	void updateLists(bool bQuietly);

	void clearAssets();


private:
	//! The current design's EditorState
	QSharedPointer<EditorState> editorState_;
	//! The current design's DesignRoot
	QSharedPointer<DesignRoot> designRoot_;
	//! The currently selected Analysis Asset
	QSharedPointer<Analysis> selectedAnalysis_;
	//! The currently selected Task Asset
	QSharedPointer<Task> selectedTask_;

	//! Action that gets called when the AddAnalysis button is pressed
	QAction* addAnalysisAction_;
	//! Action that gets called when the DeleteAnalysis button is pressed
	QAction* deleteAnalysisAction_;
	//! Action that gets called when the AddTask button is pressed
	QAction* addTaskAction_;
	//! Action that gets called when the EditTask button is pressed
	QAction* editTaskAction_;
	//! Action that gets called when the DeleteTask button is pressed
	QAction* deleteTaskAction_;
	//! The QComboBox that holds the Analyses that are available
	QComboBox* selectAnalysisBox_;
	//! The QComboBox that holds the Tasks that are available
	QComboBox* taskBox_;
	//! A flag to avoid recursive update calls
	bool updatingList_;
	//! A flag to avoid recursive index updates
	bool ignoreSelectBoxEvents_;
	//! An integer used to lock propagation of updates.  Only the first on gets to call the final update.
	int updateLocker_;

private slots:
	void addAnalysis();
	void deleteSelectedAnalysis();
	void selectedIndexChanged(int selectedIndex);
	void selectedItemTextChanged(const QString& selectedItemText);

	void selectedTaskIndexChanged(int selectedIndex);
	void selectedTaskTextChanged(const QString&);

	void addTask();
	void editTask();
	void deleteCurrentTask();
};
};

#endif
