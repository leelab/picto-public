#ifndef _ANALYSIS_VIEWER_H_
#define _ANALYSIS_VIEWER_H_

#include "../viewer.h"
#include "../../common/engine/pictoengine.h"
#include "../../common/compositor/PixmapVisualTarget.h"
#include "../../common/compositor/VisualTargetHost.h"
#include "../../common/network/CommandChannel.h"
#include "../../common/network/ServerDiscoverer.h"

#include <QUuid>
#include <QSqlDatabase>
using namespace Picto;

class QAction;
class QToolBar;
class QComboBox;
class QLabel;
class QTextEdit;

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
	QSharedPointer<Picto::Engine::PictoEngine> engine_;

	QSharedPointer<Picto::Experiment> experiment_;

	QAction *executeAction_;
	QAction *loadSessionAction_;
	QLabel *currSessionLabel_;
	QTextEdit *inputBox_;
	QTextEdit *outputBox_;
	QToolBar *toolBar_;

	QSqlDatabase configDb_;
	QSqlDatabase session_;

private slots:
	void loadSession();
	void executeCommand();
};

#endif