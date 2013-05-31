#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QPair>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include "AnalysisOptionWidget.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
AnalysisOptionWidget::AnalysisOptionWidget(QSharedPointer<EditorState> editorState, QWidget *parent) :
	QWidget(parent),
	editorState_(editorState),
	updatingList_(false),
	ignoreSelectBoxEvents_(false)
{
	QGroupBox* groupBox = new QGroupBox("Select Analysis");

	selectBox_ = new QComboBox();
	selectBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	selectBox_->setInsertPolicy(QComboBox::NoInsert);
	selectBox_->setEditable(false);
	connect(selectBox_,SIGNAL(currentIndexChanged(int)),this,SLOT(selectedIndexChanged(int)));
	connect(selectBox_,SIGNAL(editTextChanged(const QString&)),this,SLOT(selectedItemTextChanged(const QString&)));

	addAnalysisAction_ = new QAction(QIcon(":/icons/newanalysis.svg"),
									tr("Add Analysis"),this);
	addAnalysisAction_->setStatusTip(tr("Add Analysis"));
	addAnalysisAction_->setEnabled(true);
	connect(addAnalysisAction_,SIGNAL(triggered()),this,SLOT(addAnalysis()));
	QToolButton* addAnalysisButton = new QToolButton();
	addAnalysisButton->setDefaultAction(addAnalysisAction_);
	addAnalysisButton->setAutoRaise(true);

	deleteAnalysisAction_ = new QAction(QIcon(":/icons/delete.png"),
                               tr("Delete Analysis"), this);
    deleteAnalysisAction_->setStatusTip(tr("Delete Analysis"));
	deleteAnalysisAction_->setEnabled(false);
	connect(deleteAnalysisAction_,SIGNAL(triggered()),this,SLOT(deleteSelectedAnalysis()));
	QToolButton* deleteAnalysisButton = new QToolButton();
	deleteAnalysisButton->setDefaultAction(deleteAnalysisAction_);
	deleteAnalysisButton->setAutoRaise(true);

	QHBoxLayout* buttonLayout(new QHBoxLayout());
	buttonLayout->addWidget(addAnalysisButton);
	buttonLayout->addWidget(deleteAnalysisButton);

	QHBoxLayout* groupBoxLayout(new QHBoxLayout());
	groupBoxLayout->addWidget(selectBox_);
	groupBoxLayout->addLayout(buttonLayout);

	groupBox->setLayout(groupBoxLayout);

	QHBoxLayout* widgetLayout(new QHBoxLayout());
	widgetLayout->addWidget(groupBox);

	setLayout(widgetLayout);
}

void AnalysisOptionWidget::setDesignRoot(QSharedPointer<DesignRoot> designRoot)
{
	if(!isEnabled())
		return;
	if(designRoot_)
	{
		disconnect(designRoot_.data(),SIGNAL(refreshedFromXml()),this,SLOT(updateAnalysisList()));
	}
	designRoot_ = designRoot;
	if(designRoot_)
	{
		connect(designRoot_.data(),SIGNAL(refreshedFromXml()),this,SLOT(updateAnalysisList()));
	}
	updateAnalysisList();
}

void AnalysisOptionWidget::setSelectedAnalysis(QUuid analysisId)
{
	if(!isEnabled())
		return;
	if(analysisId == QUuid())
	{
		selectBox_->setCurrentIndex(0);
	}
	else
	{
		for(int i=1;i<selectBox_->count();i++)
		{
			QUuid currAnalysisId = selectBox_->itemData(i).toUuid();
			if(selectBox_->itemData(i).toUuid() == analysisId)
			{
				selectBox_->setCurrentIndex(i);
				break;
			}
		}
	}
}

void AnalysisOptionWidget::setSelectedAnalysis(QSharedPointer<Analysis> analysis)
{
	if(!isEnabled())
		return;
	QUuid id;
	if(analysis)
		id = analysis->getAssociateId();
	setSelectedAnalysis(id);
}

QSharedPointer<Analysis> AnalysisOptionWidget::getSelectedAnalysis()
{
	return selectedAnalysis_;
}

QUuid AnalysisOptionWidget::getSelectedAnalysisId()
{
	QSharedPointer<Analysis> selectedAnalysis = getSelectedAnalysis();
	if(selectedAnalysis)
		return selectedAnalysis->getAssociateId();
	return QUuid();
}

void AnalysisOptionWidget::updateAnalysisList()
{
	if(!isEnabled())
		return;
	if(updatingList_)
		return;
	updatingList_ = true;
	QUuid lastAnalysisId;
	if(selectedAnalysis_)
		lastAnalysisId = selectedAnalysis_->getAssociateId();
	ignoreSelectBoxEvents_ = true;
	int newSelectionIndex = 0;
	selectBox_->clear();
	selectBox_->addItem("None");
	QUuid currAnalysisId;
	for(int i=0;i<designRoot_->getNumAnalyses();i++)
	{
		currAnalysisId = designRoot_->getAnalysis(i).staticCast<Analysis>()->getAssociateId();
		selectBox_->addItem(designRoot_->getAnalysis(i)->getName(),currAnalysisId);
		if(!lastAnalysisId.isNull() && currAnalysisId == lastAnalysisId)
			newSelectionIndex = i+1;
	}
	//We set the current index to -1 so that whatever the newSelectionIndex is, it will
	//cause selectedIndexChanged to get triggered.
	selectBox_->setCurrentIndex(-1);
	ignoreSelectBoxEvents_ = false;
	selectBox_->setCurrentIndex(newSelectionIndex);
	updatingList_ = false;
}

void AnalysisOptionWidget::addAnalysis()
{
	if(!isEnabled())
		return;
	QSharedPointer<Asset> newAnalysis;

	//Add a new Analysis Design to the designRoot
	newAnalysis = designRoot_->importAnalysis("<Analysis/>");
	//Create UI Data for the new Analysis and attach it
	AssociateRootHost* assocRootHost = dynamic_cast<AssociateRootHost*>(newAnalysis.data());
	Q_ASSERT(assocRootHost);
	QUuid hostId = assocRootHost->getHostId();
	QSharedPointer<Asset> newUIData = newAnalysis->getParentAsset().staticCast<DataStore>()->createChildAsset("UIData",QString(),QString());
	QString feedback;
	newUIData.staticCast<AssociateRoot>()->LinkToAsset(newAnalysis,feedback);
	Q_ASSERT(newAnalysis);

	//Update list to include new analysis
	updateAnalysisList();

	//Select new analysis
	setSelectedAnalysis(newAnalysis.staticCast<Analysis>()->getAssociateId());

	//Select name of new analysis for editing
	selectBox_->lineEdit()->selectAll();
}

void AnalysisOptionWidget::selectedIndexChanged(int selectedIndex)
{
	if(!isEnabled())
		return;
	if(ignoreSelectBoxEvents_)
		return;

	Q_ASSERT(designRoot_);
	Q_ASSERT(selectedIndex <= designRoot_->getNumAnalyses());

	QSharedPointer<Analysis> lastCurrAnalysis = editorState_->getCurrentAnalysis();

	//If no analysis was selected
	if(selectedIndex <= 0)
	{
		selectBox_->setEditable(false);
		deleteAnalysisAction_->setEnabled(false);

		//Erase the current selected analysis pointer
		selectedAnalysis_.clear();
	}
	else
	{
		selectBox_->setEditable(true);
		deleteAnalysisAction_->setEnabled(true);

		//Update the current selected analysis
		selectedAnalysis_ = designRoot_->getAnalysis(selectedIndex-1).staticCast<Analysis>();
	}
	if(selectedAnalysis_ != lastCurrAnalysis)
	{
		//Tell the editorState about the new current analysis
		editorState_->setCurrentAnalysis(selectedAnalysis_);
	}
}

void AnalysisOptionWidget::selectedItemTextChanged(const QString& selectedItemText)
{
	if(!isEnabled())
		return;
	if(ignoreSelectBoxEvents_)
		return;
	int currIndex = selectBox_->currentIndex();
	if(currIndex <= 0)
		return;
	Q_ASSERT(currIndex >= 0 && currIndex <= designRoot_->getNumAnalyses());
	if(selectedItemText != designRoot_->getAnalysis(currIndex-1)->getName())
	{
		//Update the analysis' name
		designRoot_->getAnalysis(currIndex-1).staticCast<Analysis>()->setName(selectedItemText);

		//Update the list to reflect the new name
		updateAnalysisList();
	}
}

void AnalysisOptionWidget::deleteSelectedAnalysis()
{
	if(!isEnabled())
		return;
	Q_ASSERT(editorState_ && editorState_->getTopLevelAsset());
	QSharedPointer<Analysis> selectedAnalysis = getSelectedAnalysis();
	if(!selectedAnalysis)
		return;

	int ret = QMessageBox::warning(this, tr("Picto"),
                                QString("Are you sure you want to delete the %1 Analysis?").arg(selectedAnalysis->getName()),
                                QMessageBox::Yes | QMessageBox::Cancel,
                                QMessageBox::Cancel);
	if(ret != QMessageBox::Yes)
		return;

	//Detach analysis from experiment
	QUuid currAnalysisId = selectedAnalysis->getAssociateId();
	editorState_->getTopLevelAsset().staticCast<DataStore>()->ClearAssociateDescendants(currAnalysisId);
	
	//Remove analysis from design
	int analysisIndexToRemove = selectBox_->currentIndex()-1;
	Q_ASSERT(analysisIndexToRemove < designRoot_->getNumAnalyses());
	if(!designRoot_->removeAnalysis(analysisIndexToRemove))
		Q_ASSERT(false);

	//Reset experiment so that the removal of the analysis (setDeleted() is used) takes affect.
	//This will result in updateAnalysisList() getting called.
	editorState_->triggerExperimentReset();

	//Set current analysis to none.
	setSelectedAnalysis(QUuid());
}