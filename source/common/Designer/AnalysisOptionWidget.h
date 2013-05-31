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
//! [0]
//Widget for selecting from a lit of available Analyses
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
	void createNewAnalysis();

	QSharedPointer<EditorState> editorState_;
	QSharedPointer<DesignRoot> designRoot_;
	QSharedPointer<Analysis> selectedAnalysis_;
	QAction* deleteAnalysisAction_;
	QComboBox* selectBox_;
	bool updatingList_;
private slots:
	void selectedIndexChanged(int selectedIndex);
	void selectedItemTextChanged(const QString& selectedItemText);
	void deleteSelectedAnalysis();
};
};
//! [0]
#endif
