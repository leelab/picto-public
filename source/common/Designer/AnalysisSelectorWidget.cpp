#include <QCheckBox>
#include <QVBoxLayout>
#include "AnalysisSelectorWidget.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
AnalysisSelectorWidget::AnalysisSelectorWidget(QWidget *parent) :
	QWidget(parent)
{
	selectGroup_.setExclusive(false);
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
	for(int i=0;i<selectGroup_.buttons().size();i++)
	{
		if(selectGroup_.button(i)->isChecked())
		{
			Q_ASSERT(i<designRoot_->getNumAnalyses());
			returnVal.append(designRoot_->getAnalysis(i).staticCast<Analysis>()->getAssociateId());
		}
	}
	return returnVal;
}

void AnalysisSelectorWidget::updateAnalysisList()
{
	selectedAnalyses_.clear();
	foreach(QAbstractButton* button,selectGroup_.buttons())
	{
		selectGroup_.removeButton(button);
	}
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
		selectGroup_.addButton(analysisCheckbox,i);
		layout->addWidget(analysisCheckbox);
	}
	selectBox_->setLayout(layout);
}