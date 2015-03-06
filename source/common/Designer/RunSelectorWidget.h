#ifndef RunSelectorWidget_H
#define RunSelectorWidget_H

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

/*! \brief A list of selectable Task runs for use with the Playback Viewer.
 *	\details This widget is populated by calling addRun with the name of a Task run from a session, whether
 *	the run was saved, its index within the session, notes that were saved with it, and the file path of the
 *	session file containing the run.  The widget will then display a list of runs, grouped by session.  Runs'
 *	tooltips will show the saved notes, and they can have a RunSelectorWidget::RunStatus set to them which
 *	will cause them to be highlighted according to the current status and optionally change their tooltip
 *	temporarily to an input message.  Selected runs can be retrieved with getSelectedRuns(), a runSelectionChanged()
 *	signal is emitted when the selection changes, and other functions are available for additional data retrieval.
 *	Convenience buttons are included for selecting all runs, clearing all runs and selecting only saved runs.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API RunSelectorWidget : public QWidget
#else
class RunSelectorWidget : public QWidget
#endif
{
    Q_OBJECT

public:
	RunSelectorWidget(QWidget *parent = NULL);
	virtual ~RunSelectorWidget(){};
	void addRun(bool saved,QString filePath,QString runName,int index,QString notes = "");
	void clear();
	QList<int> getSelectedRuns(QString filePath);
	QStringList getSelectedFilePaths();
	int getNumSelectedRuns();
	/*! \brief An enum indicating the playback Status of a particular run.*/
	enum RunStatus {
		IDLE,			//!< Nothing is happening with this run.
		INPROGRESS,		//!< This run is currently being played back.
		COMPLETE,		//!< This run was played back and is now complete.
		ERROROCCURED	//!< An attempt was made to play this run back, but an error occured and it failed.
		};
	void setRunStatus(QString fileName,int runIndex,RunStatus status,QString message="");
	int getRunStatus(QString fileName,int runIndex);
	void resetAllRunStatus();
	void selectRun(QString fileName,int runIndex);
	void deselectRun(QString fileName,int runIndex);
	void deselectAll();
	void enableCheckBoxes(bool enabled);
signals:
	/*! \brief Emitted when the selection changed somehow (a run was selected or deselected) in the RunSelectorWidget.*/
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
	/*! \brief A simple struct used for storing information about a particular run.*/
	struct RunInfo
	{
		/*! \brief Constructs a new RunInfo with default values.*/
		RunInfo(){fileName_ = "";index_ = -1;runStatus_ = IDLE;saved_ = false;button_=NULL;notes_="";};
		/*! \brief Constructs a new RunInfo with the input values.*/
		RunInfo(QString fileName,int index,bool saved,QAbstractButton* button,QString notes = "",RunStatus runStatus = IDLE){fileName_ = fileName;index_ = index;saved_ = saved;runStatus_ = runStatus;button_ = button;notes_=notes;};
		QString fileName_;			//!< The path of the session file where this run is stored. 
		int index_;					//!< The index of the run in its session.
		RunStatus runStatus_;		//!< The RunSelectorWidget::RunStatus of the run.
		bool saved_;				//!< True if this run was saved in the session.
		QString notes_;				//!< Notes saved by the operator into the session for this run.
		QAbstractButton* button_;	//!< The checkbox used to select this run.
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

#endif
