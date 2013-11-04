#include <QCheckBox>
#include <QVBoxLayout>
#include <QLabel>
#include "AnalysisSelectorWidget.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
AnalysisSelectorWidget::AnalysisSelectorWidget(QWidget *parent) :
	QWidget(parent)
{
	tabWidget_ = new QTabWidget();
	analysesBox_ = new QWidget();
	
	analysesForImportBox_ = new QWidget();
		
	tabWidget_->addTab(analysesBox_,"Session");
	tabWidget_->addTab(analysesForImportBox_,"Import");

	QVBoxLayout* layout(new QVBoxLayout());
	layout->addWidget(tabWidget_);
	setLayout(layout);
}

void AnalysisSelectorWidget::clearLocalAnalyses()
{
	localAnalysisLookup_.clear();
	currFilePath_ = "";
	updateLocalAnalysisList();
}

void AnalysisSelectorWidget::setLocalDesignRoot(QString filePath,QSharedPointer<DesignRoot> designRoot)
{
	if(designRoot.isNull() || filePath.isEmpty())
	{	
		return;
	}
	QList<AnalysisInfo> localAnalysisList;
	for(int i=0;i<designRoot->getNumAnalyses();i++)
	{
		localAnalysisList.append(
			AnalysisInfo(	designRoot->getAnalysis(i).staticCast<Analysis>()->getAssociateId(),
				designRoot->getAnalysis(i)->getName()
			)
		);
		//analysisNames_.append(designRoot->getAnalysis(i)->getName());
		//analysisIds_.append(designRoot->getAnalysis(i).staticCast<Analysis>()->getAssociateId());
	}
	localAnalysisLookup_[filePath] = localAnalysisList;
}

void AnalysisSelectorWidget::setLocalDesignAnalyses(QString filePath,QList<QUuid> analysisIds,QStringList analysisNames)
{
	if(filePath.isEmpty())
	{	
		return;
	}
	QList<AnalysisInfo> localAnalysisList;
	for(int i=0;i<analysisIds.size();i++)
	{
		localAnalysisList.append(
			AnalysisInfo(analysisIds[i],analysisNames[i]
			)
		);
		//analysisNames_.append(designRoot->getAnalysis(i)->getName());
		//analysisIds_.append(designRoot->getAnalysis(i).staticCast<Analysis>()->getAssociateId());
	}
	localAnalysisLookup_[filePath] = localAnalysisList;
}

void AnalysisSelectorWidget::setDesignRootForImport(QSharedPointer<DesignRoot> designRoot)
{
	analysesForImport_.clear();
	if(designRoot.isNull())
	{	
		updateAnalysesForImportList();
		return;
	}
	for(int i=0;i<designRoot->getNumAnalyses();i++)
	{
		analysesForImport_.append(
			AnalysisInfo(	designRoot->getAnalysis(i).staticCast<Analysis>()->getAssociateId(),
				designRoot->getAnalysis(i)->getName()
			)
		);
	}
	updateAnalysesForImportList();
}

void AnalysisSelectorWidget::setCurrentFile(QString filePath)
{
	currFilePath_ = filePath;
	updateLocalAnalysisList();
}

QList<QUuid> AnalysisSelectorWidget::getSelectedAnalysisIds()
{
	QList<QUuid> returnVal;
	if(currFilePath_.isEmpty() || !localAnalysisLookup_.contains(currFilePath_))
		return returnVal;
	for(int i=0;i<localAnalysisLookup_[currFilePath_].size();i++)
	{
		QAbstractButton* button = qobject_cast<QAbstractButton*>(analysesBox_->layout()->itemAt(i)->widget());
		Q_ASSERT(button);
		if(button->isChecked())
		{
			returnVal.append(localAnalysisLookup_[currFilePath_][i].id_);
		}
	}
	return returnVal;
}

QList<QUuid> AnalysisSelectorWidget::getSelectedAnalysisIdsForImport()
{
	QList<QUuid> returnVal;
	for(int i=0;i<analysesForImport_.size();i++)
	{
		QAbstractButton* button = qobject_cast<QAbstractButton*>(analysesForImportBox_->layout()->itemAt(i)->widget());
		Q_ASSERT(button);
		if(button->isChecked())
		{
			returnVal.append(analysesForImport_[i].id_);
		}
	}
	return returnVal;
}

bool AnalysisSelectorWidget::hasSelectedIds()
{
	return bool(getSelectedAnalysisIdsForImport().size()) || bool(getSelectedAnalysisIds().size());
}


void AnalysisSelectorWidget::updateLocalAnalysisList()
{
	//Remove all old buttons from layout.  We do this in a roundabout but simple way.  Moving a widget's
	//layout to another widget and then destroying that widget destroys the layout and all the children that
	//were attached to it.
	if(analysesBox_->layout())
		QWidget().setLayout(analysesBox_->layout());
	analysesBox_->setLayout(new QVBoxLayout());

	//Add new buttons to layout
	if(localAnalysisLookup_.contains(currFilePath_))
	{
		for(int i=0;i<localAnalysisLookup_[currFilePath_].size();i++)
		{
			QCheckBox* analysisCheckbox(new QCheckBox(localAnalysisLookup_[currFilePath_][i].name_));
			connect(analysisCheckbox,SIGNAL(clicked(bool)),this,SLOT(checkboxChanged(bool)));
			qobject_cast<QVBoxLayout*>(analysesBox_->layout())->addWidget(analysisCheckbox,0,Qt::AlignTop);
			qobject_cast<QVBoxLayout*>(analysesBox_->layout())->setStretch(i,0);
		}
	}
	qobject_cast<QVBoxLayout*>(analysesBox_->layout())->addStretch(1);
	//Enable this tab if it contains any analyses
	tabWidget_->setTabEnabled(0,localAnalysisLookup_.contains(currFilePath_) && localAnalysisLookup_[currFilePath_].size());
	emit analysisWidgetChanged();
}

void AnalysisSelectorWidget::updateAnalysesForImportList()
{
	//Remove old buttons from layout
	if(analysesForImportBox_->layout())
		QWidget().setLayout(analysesForImportBox_->layout());
	analysesForImportBox_->setLayout(new QVBoxLayout());

	//Add new buttons to layout
	for(int i=0;i<analysesForImport_.size();i++)
	{
		QCheckBox* analysisCheckbox(new QCheckBox(analysesForImport_[i].name_));
		connect(analysisCheckbox,SIGNAL(clicked(bool)),this,SLOT(checkboxChanged(bool)));
		qobject_cast<QVBoxLayout*>(analysesForImportBox_->layout())->addWidget(analysisCheckbox,0,Qt::AlignTop);
		qobject_cast<QVBoxLayout*>(analysesForImportBox_->layout())->setStretch(i,0);
	}
	qobject_cast<QVBoxLayout*>(analysesForImportBox_->layout())->addStretch(1);
	//Enable this tab if it contains any analyses
	tabWidget_->setTabEnabled(1,analysesForImport_.size());
	emit analysisWidgetChanged();
}

void AnalysisSelectorWidget::checkboxChanged(bool)
{
	emit analysisWidgetChanged();
}