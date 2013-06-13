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
	layout->addWidget(new QLabel("Select Analyses"));
	layout->addWidget(tabWidget_);
	setLayout(layout);
}

void AnalysisSelectorWidget::setDesignRoot(QSharedPointer<DesignRoot> designRoot)
{
	analysisIds_.clear();
	analysisNames_.clear();
	if(designRoot.isNull())
	{	
		updateAnalysisList();
		return;
	}
	for(int i=0;i<designRoot->getNumAnalyses();i++)
	{
		analysisNames_.append(designRoot->getAnalysis(i)->getName());
		analysisIds_.append(designRoot->getAnalysis(i).staticCast<Analysis>()->getAssociateId());
	}
	updateAnalysisList();
}

void AnalysisSelectorWidget::setDesignRootForImport(QSharedPointer<DesignRoot> designRoot)
{
	analysisNamesForImport_.clear();
	analysisIdsForImport_.clear();
	if(designRoot.isNull())
	{	
		updateAnalysisList();
		return;
	}
	for(int i=0;i<designRoot->getNumAnalyses();i++)
	{
		analysisNamesForImport_.append(designRoot->getAnalysis(i)->getName());
		analysisIdsForImport_.append(designRoot->getAnalysis(i).staticCast<Analysis>()->getAssociateId());
	}
	updateAnalysisList();
}

QList<QUuid> AnalysisSelectorWidget::getSelectedAnalysisIds()
{
	QList<QUuid> returnVal;
	for(int i=0;i<analysisIds_.size();i++)
	{
		QAbstractButton* button = qobject_cast<QAbstractButton*>(analysesBox_->layout()->itemAt(i)->widget());
		Q_ASSERT(button);
		if(button->isChecked())
		{
			returnVal.append(analysisIds_[i]);
		}
	}
	return returnVal;
}

QList<QUuid> AnalysisSelectorWidget::getSelectedAnalysisIdsForImport()
{
	QList<QUuid> returnVal;
	for(int i=0;i<analysisIdsForImport_.size();i++)
	{
		QAbstractButton* button = qobject_cast<QAbstractButton*>(analysesForImportBox_->layout()->itemAt(i)->widget());
		Q_ASSERT(button);
		if(button->isChecked())
		{
			returnVal.append(analysisIdsForImport_[i]);
		}
	}
	return returnVal;
}

void AnalysisSelectorWidget::updateAnalysisList()
{
	//Remove all old buttons from layout.  We do this in a roundabout but simple way.  Moving a widget's
	//layout to another widget and then destroying that widget destroys the layout and all the children that
	//were attached to it.
	if(analysesBox_->layout())
		QWidget().setLayout(analysesBox_->layout());
	analysesBox_->setLayout(new QVBoxLayout());

	//Add new buttons to layout
	for(int i=0;i<analysisNames_.size();i++)
	{
		QCheckBox* analysisCheckbox(new QCheckBox(analysisNames_[i]));
		qobject_cast<QVBoxLayout*>(analysesBox_->layout())->addWidget(analysisCheckbox,0,Qt::AlignTop);
		qobject_cast<QVBoxLayout*>(analysesBox_->layout())->setStretch(i,0);
	}
	qobject_cast<QVBoxLayout*>(analysesBox_->layout())->addStretch(1);
	//Enable this tab if it contains any analyses
	tabWidget_->setTabEnabled(0,analysisNames_.size());

	//Remove old buttons from layout
	if(analysesForImportBox_->layout())
		QWidget().setLayout(analysesForImportBox_->layout());
	analysesForImportBox_->setLayout(new QVBoxLayout());

	//Add new buttons to layout
	for(int i=0;i<analysisNamesForImport_.size();i++)
	{
		QCheckBox* analysisCheckbox(new QCheckBox(analysisNamesForImport_[i]));
		qobject_cast<QVBoxLayout*>(analysesForImportBox_->layout())->addWidget(analysisCheckbox,0,Qt::AlignTop);
		qobject_cast<QVBoxLayout*>(analysesForImportBox_->layout())->setStretch(i,0);
	}
	qobject_cast<QVBoxLayout*>(analysesForImportBox_->layout())->addStretch(1);
	//Enable this tab if it contains any analyses
	tabWidget_->setTabEnabled(1,analysisNamesForImport_.size());
}