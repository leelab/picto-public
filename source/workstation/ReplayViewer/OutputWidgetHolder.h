#ifndef _OutputWidgetHolder_H_
#define _OutputWidgetHolder_H_

#include <QFrame>
#include <QUuid>
#include <QPushButton>
#include "../../common/parameter/AnalysisOutputWidgetContainer.h"

class QTimer;

/*!	\brief A widget that displays an OutputWidgetContainer with its contained AnalysisOutputWidget objects and provides
 *	functionality for saving the AnalysisOutputWidgets' data to disk.
 *	\details This widget is used in both the ReplayViewer and TestViewer.  It gives the operator a way to get a quick view of 
 *	AnalysisOutput results and save them to disk.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class OutputWidgetHolder : public QFrame
{
	Q_OBJECT
public:
	OutputWidgetHolder(QWidget *parent=0);
	virtual ~OutputWidgetHolder();
	void newRunStarted(QUuid runId);
	void setSaveParameters(QString saveToPath,bool separateSubDirs);
	void saveOutput();
public slots:
	void update();
private:
	void resetLayout();
	Picto::AnalysisOutputWidgetContainer* containerWidget_;
	QPushButton* saveButton_;
	QUuid latestRunId_;
	QTimer* updateTimer_;
	QString saveToPath_;
	bool separateSubDirs_;
private slots:
	void saveOutputFromDialog();
	void enableSaveButton();
};

#endif