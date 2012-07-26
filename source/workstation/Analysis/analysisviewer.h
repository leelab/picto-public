#ifndef _ANALYSIS_VIEWER_H_
#define _ANALYSIS_VIEWER_H_

#include <QUuid>
#include <QSqlDatabase>
#include <QTabWidget>

#include "../viewer.h"
#include "../../common/engine/pictoengine.h"
#include "../../common/compositor/PixmapVisualTarget.h"
#include "../../common/compositor/VisualTargetHost.h"
#include "../../common/network/CommandChannel.h"
#include "../../common/network/ServerDiscoverer.h"
#include "AnalysisOutputDisplay.h"
#include "AnalysisParameterSelector.h"
#include "TaskRunSelector.h"
#include "../../common/analysis/AnalysisContainer.h"
#include "../Designer/Designer.h"

using namespace Picto;

class QAction;
class QToolBar;
class QComboBox;
class QLabel;
class QTextEdit;
class QLineEdit;
class QProgressBar;

/*!	\brief	This views and controls a remotely running experiment
 *
 *	This is the viewer that will be used while an experiment is running.  It has the
 *	following features:
 *		Allows starting, stopping, and pausing of remotely running experiments
 *		Displays behavioral input (eye tracker) as a moving cursor
 *		Displays current state of remote experiment
 */
class AnalysisViewer : public Viewer
{
	Q_OBJECT
public:
	AnalysisViewer(QWidget *parent=0);
	virtual ~AnalysisViewer();

	QString type() { return "Remote"; };
	bool aboutToQuit();

public slots:
	void init();  //Called just before displaying the viewer
	void deinit();	//Called just after the user switches out of the viewer

private:

	void setupUi();
	int getElementId(QString path);
	int getPropertyId(int parentId, QString name);
	void resetQueryParameterData();
	void getQueryParameters();

	QString getTransitionIds(int parentId, QString name, bool asSource);
	QSharedPointer<Picto::Engine::PictoEngine> engine_;

	QSharedPointer<Picto::Experiment> experiment_;
	QSharedPointer<AnalysisContainer> analysisContainer_; 
	Designer* anaDesigner_;

	QAction *executeAction_;
	QAction *loadSessionAction_;
	QAction *saveOutputAction_;
	QLabel *currSessionLabel_;
	AnalysisOutputDisplay *outputDisplay_;
	
	QTabWidget* mainTabWindow_;
	TaskRunSelector* runSelector_;
	int runsRemaining_;
	
	QToolBar *toolBar_;
	QProgressBar *progressBar_;
	enum {ANALYZING,SAVING} status_;

	QSqlDatabase configDb_;
	QList<QSqlDatabase> sessions_;

	QTimer* progressBarTimer_;

private slots:
	void loadSession();
	void saveOutput();
	void executeCommand();

	void updateUI();
	void updateProgressBar();
};

#endif