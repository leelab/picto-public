#ifndef _ANALYSIS_OUTPUT_WIDGET_CONTAINER_H_
#define _ANALYSIS_OUTPUT_WIDGET_CONTAINER_H_

#include <QWidget>
#include <QToolBox>
#include <QDir>
#include <QUuid>
#include "../common.h"
#include "AnalysisOutputWidget.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API AnalysisOutputWidgetContainer : public QWidget
#else
class AnalysisOutputWidgetContainer : public QWidget
#endif
{
	Q_OBJECT
public:
	AnalysisOutputWidgetContainer(QString runName, QWidget* parent = 0);
	virtual ~AnalysisOutputWidgetContainer();

	virtual bool isSaveable();
	virtual bool saveOutputTo(QDir directory, bool separateRunDirs);
signals:
	void detectedSaveable();	//Emitted when widget first detects that it has
						//savable child widgets.

protected:
	void addWidget(QUuid identifier, AnalysisOutputWidget* widget);
	friend class AnalysisOutput;
private:
	QHash<QUuid,AnalysisOutputWidget*> widgetLookup_;
	QHash<QUuid,bool> enabledLookup_;
	QToolBox* tabWidget_;
	double fractionOutputSaved_;
	QString runName_;
private slots:
	void widgetIsValid(QUuid widgetIdentifier);
}; //namespace Picto
};
#endif