#ifndef AnalysisSelectorWidget_H
#define AnalysisSelectorWidget_H

#include <QWidget>
#include <QSharedPointer>
#include <QGroupBox>
#include <QButtonGroup>
#include <QMap>
#include "../design/DesignRoot.h"
#include "../parameter/Analysis.h"

namespace Picto {
//! [0]
//Widget for selecting from a lit of available Analyses
#if defined WIN32 || defined WINCE
class PICTOLIB_API AnalysisSelectorWidget : public QWidget
#else
class AnalysisSelectorWidget : public QWidget
#endif
{
    Q_OBJECT

public:
   AnalysisSelectorWidget(QWidget *parent = NULL);
   virtual ~AnalysisSelectorWidget(){};
   void setDesignRoot(QSharedPointer<DesignRoot> designRoot);
   QList<QUuid> getSelectedAnalysisIds();
signals:
   void selectedAnalysesChanged();
private:
	QSharedPointer<DesignRoot> designRoot_;
	QMap<int,QSharedPointer<Analysis>> selectedAnalyses_;
	QGroupBox* selectBox_;
	QButtonGroup selectGroup_;
private slots:
	void updateAnalysisList();
};
};
//! [0]
#endif
