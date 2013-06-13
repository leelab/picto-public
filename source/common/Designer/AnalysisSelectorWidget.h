#ifndef AnalysisSelectorWidget_H
#define AnalysisSelectorWidget_H

#include <QWidget>
#include <QSharedPointer>
#include <QTabWidget>
#include <QGroupBox>
#include <QAbstractButton>
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
	void setDesignRootForImport(QSharedPointer<DesignRoot> designRoot);
	QList<QUuid> getSelectedAnalysisIds();
	QList<QUuid> getSelectedAnalysisIdsForImport();
private:
	QTabWidget* tabWidget_;
	QWidget* analysesBox_;
	QWidget* analysesForImportBox_;
	QStringList	analysisNames_;	
	QStringList	analysisNamesForImport_;	
	QList<QUuid> analysisIds_;
	QList<QUuid> analysisIdsForImport_;
private slots:
	void updateAnalysisList();
};
};
//! [0]
#endif
