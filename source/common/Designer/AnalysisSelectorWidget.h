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
/*! \brief A widget used to select Analysis designs to apply to a test run or session playback.
 *	\details This widget includes two tabs, one including built in Analyses (ie. Analyses that are saved as part of a
 *	design file) and the other including importable Analyses (ie. Analyses that the we can attempt to apply to previously
 *	run sessions).  Each tab contains check boxes such that multiple analyses can be selected at once.  The list of
 *	Analyses may be built up automatically based on DesignRoot objects passed into setDesignRootForImport() and
 *	setLocalDesignRoot or added manually using setLocalDesignAnalyses().  For the purposes of easily switching between
 *	playback of multiple designs, the widget also lets us call setLocalDesignRoot() multiple times for different
 *	design/session files, then choose which Local Analyses we would like to display in the widget by using
 *	setCurrentFile().  Only one "importable" DesignRoot functions at a time however.
 *
 *	Selected Analysis Ids are retrieved by using getSelectedAnlysisIds() for local Analyses and
 *	getSelectedAnalysisIdsForImport() for importable Analyses.
 *	\note This widget performs no import compatibility tests.  Whether or not the importable Analyses are compatible with
 *	the current Design must be tested elsewhere.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
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
	void enableCheckboxes(bool enable);
	bool hasSelectedIds();

signals:
	void analysisWidgetChanged();	//Triggered whenever the widget changes (ie.  Something clicked, buttons removed, added, etc)

private:
	QTabWidget* tabWidget_;
	QWidget* analysesBox_;
	QWidget* analysesForImportBox_;
	//! Stores Analysis ID, Analysis Name pairs.
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
	void updateLocalAnalysisList();
	void updateAnalysesForImportList();

private slots:
	void checkboxChanged(bool checked);
};
};

#endif
