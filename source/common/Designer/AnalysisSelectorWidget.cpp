#include <QCheckBox>
#include <QVBoxLayout>
#include "AnalysisSelectorWidget.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
AnalysisSelectorWidget::AnalysisSelectorWidget(QWidget *parent) :
	QWidget(parent)
{
	selectBox_ = new QGroupBox("Select Analysis");
	QVBoxLayout* layout(new QVBoxLayout());
	layout->addWidget(selectBox_);
	setLayout(layout);
}

void AnalysisSelectorWidget::setDesignRoot(QSharedPointer<DesignRoot> designRoot)
{
	designRoot_ = designRoot;
	updateAnalysisList();
}

QList<QUuid> AnalysisSelectorWidget::getSelectedAnalysisIds()
{
	QList<QUuid> returnVal;
	foreach(QSharedPointer<Analysis> analysis,selectedAnalyses_)
	{
		returnVal.append(analysis->getAssociateId());
	}
	return returnVal;
}

void AnalysisSelectorWidget::updateAnalysisList()
{
	delete selectBox_->layout();
	if(!designRoot_)
	{
		selectBox_->setLayout(new QVBoxLayout());
		return;
	}
	QVBoxLayout* layout(new QVBoxLayout());
	for(int i=0;i<designRoot_->getNumAnalyses();i++)
	{
		QCheckBox* analysisCheckbox(new QCheckBox(designRoot_->getAnalysis(i)->getName()));
		layout->addWidget(analysisCheckbox);
	}
	selectBox_->setLayout(layout);
}