#ifndef _ANALYSIS_OUTPUT_DISPLAY_H_
#define _ANALYSIS_OUTPUT_DISPLAY_H_

#include <QWidget>
#include <QSqlQuery>
#include <QTabWidget>
#include <QVBoxLayout>

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
	void addOutputTab(QString name,QWidget* widget);
	void clear();

private:
	QTabWidget *outputBox_;
	QVBoxLayout *mainLayout_;
};

#endif