#ifndef AnalysisSelectorWidget_H
#define AnalysisSelectorWidget_H

#include <QWidget>
#include <QSharedPointer>
#include <QTabWidget>
#include <QGroupBox>
#include <QAbstractButton>
#include <QHash>
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
	void clearLocalAnalyses();
	void setLocalDesignRoot(QString filePath,QSharedPointer<DesignRoot> designRoot);
	void setLocalDesignAnalyses(QString filePath,QList<QUuid> analysisIds,QStringList analysisNames);
	void setDesignRootForImport(QSharedPointer<DesignRoot> designRoot);
	void setCurrentFile(QString filePath);
	QList<QUuid> getSelectedAnalysisIds();
	QList<QUuid> getSelectedAnalysisIdsForImport();
	bool hasSelectedIds();
signals:
	void analysisWidgetChanged();	//Triggered whenever the widget changes (ie.  Something clicked, buttons removed, added, etc)
private:
	QTabWidget* tabWidget_;
	QWidget* analysesBox_;
	QWidget* analysesForImportBox_;
	struct AnalysisInfo
	{
		AnalysisInfo(){id_ = QUuid();name_ = "";};
		AnalysisInfo(QUuid id,QString name){id_ = id;name_ = name;};
		QUuid id_;
		QString name_;
	};
	QHash<QString,QList<AnalysisInfo>> localAnalysisLookup_;
	QList<AnalysisInfo> analysesForImport_;
	QString currFilePath_;
	//QStringList	analysisNames_;	
	//QStringList	analysisNamesForImport_;	
	//QList<QUuid> analysisIds_;
	//QList<QUuid> analysisIdsForImport_;
	void updateLocalAnalysisList();
	void updateAnalysesForImportList();
private slots:
	void checkboxChanged(bool checked);
};
};
//! [0]
#endif
