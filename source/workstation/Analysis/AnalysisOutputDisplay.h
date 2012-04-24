#ifndef _ANALYSIS_OUTPUT_DISPLAY_H_
#define _ANALYSIS_OUTPUT_DISPLAY_H_

#include <QWidget>
#include <QSqlQuery>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QDir>

/*!	\brief	This views and controls a remotely running experiment
 *
 *	This is the viewer that will be used while an experiment is running.  It has the
 *	following features:
 *		Allows starting, stopping, and pausing of remotely running experiments
 *		Displays behavioral input (eye tracker) as a moving cursor
 *		Displays current state of remote experiment
 */
class AnalysisOutputDisplay : public QWidget
{
	Q_OBJECT
public:
	AnalysisOutputDisplay(QWidget *parent=0);
	virtual ~AnalysisOutputDisplay();
	int addTopLevelTab(QString name);
	void addSubTab(int topLevelIndex,QString name,QWidget* widget);
	bool supportsSaving();
	bool saveOutputToDirectory(QDir directory);
	void clear();

private:
	QTabWidget *topLevelTabs_;
	QVBoxLayout *mainLayout_;
};

#endif