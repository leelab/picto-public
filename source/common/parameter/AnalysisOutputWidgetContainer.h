#ifndef _ANALYSIS_OUTPUT_WIDGET_CONTAINER_H_
#define _ANALYSIS_OUTPUT_WIDGET_CONTAINER_H_

#include <QWidget>
#include <QToolBox>
#include <QDir>
#include <QUuid>
#include "../common.h"
#include "AnalysisOutputWidget.h"

namespace Picto {

/*! \brief A container widget that lays out all AnalysisOutputWidgets for a particular run and
 *	displays them to the operator.
 *	\details The AnalysisOutputWidgetContainer is displayed in the Picto Workstation's Playback viewer and 
 *	TestViewer.  It arranges all of the AnalysisOutputWidget objects under individual vertical tabs and 
 *	provides a save button that can be used to save the data underlying the AnalysisOutputWidgets
 *	to file.  An AnalysisOutputWidgetContainer can only be associated with a particular run and
 *	is currently created exclusively by the AnalysisOutput class in the AnalysisOutput::getContainerWidget()
 *	function.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
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
	/*! \brief Emitted when this widget first detects that it has saveable child widgets.*/
	void detectedSaveable();
						

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