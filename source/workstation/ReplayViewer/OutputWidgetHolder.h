#ifndef _OutputWidgetHolder_H_
#define _OutputWidgetHolder_H_

#include <QFrame>
#include <QUuid>
#include <QPushButton>
#include "../../common/parameter/AnalysisOutputWidgetContainer.h"

class QTimer;

/*!	\brief	Widget that shows the Analysis Output
 * 
 */
class OutputWidgetHolder : public QFrame
{
	Q_OBJECT
public:
	OutputWidgetHolder(QWidget *parent=0);
	virtual ~OutputWidgetHolder();
	void newRunStarted(QUuid runId);
public slots:
	void update();
private:
	void resetLayout();
	Picto::AnalysisOutputWidgetContainer* containerWidget_;
	QPushButton* saveButton_;
	QUuid latestRunId_;
	QTimer* updateTimer_;
private slots:
	void saveOutput();
	void enableSaveButton();
};

#endif