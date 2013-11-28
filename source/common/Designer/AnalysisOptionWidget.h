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
/*! \brief A drop down menu for selecting Analyses... currently not used.
 *	\details When we started the final version of the analysis system, we used
 *	this widget to allow users to choose which Analysis they wanted to run.
 *	As time went on, features like selecting multiple Analyses and including
 *	built-in and importable analyses in one widget became apparent and we have
 *	now moved to using the AnalysisSelectorWidget.
 *	\sa AnalysisSelectorWidget
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
public slots:
	void updateAnalysisList();
signals:
   void selectedAnalysisChanged(QSharedPointer<Analysis> analysis);
   void analysisNameChangeRequest(QSharedPointer<Analysis> analysis,QString newName);
   void newAnalysisRequested();
   void deleteAnalysisRequested();
private:
	void changeSelectedAnalysisName(QString newName);

	QSharedPointer<EditorState> editorState_;
	QSharedPointer<DesignRoot> designRoot_;
	QSharedPointer<Analysis> selectedAnalysis_;
	QAction* addAnalysisAction_;
	QAction* deleteAnalysisAction_;
	QComboBox* selectBox_;
	bool updatingList_;
	bool ignoreSelectBoxEvents_;
	bool enabled_;
private slots:
	void addAnalysis();
	void selectedIndexChanged(int selectedIndex);
	void selectedItemTextChanged(const QString& selectedItemText);
	void deleteSelectedAnalysis();
};
};

#endif
