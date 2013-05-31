#include <QCheckBox>
#include <QHBoxLayout>
#include <QToolButton>
#include <QPair>
#include "AnalysisOptionWidget.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
AnalysisOptionWidget::AnalysisOptionWidget(QSharedPointer<EditorState> editorState, QWidget *parent) :
	QWidget(parent),
	editorState_(editorState),
	updatingList_(false)
{
	selectBox_ = new QComboBox();
	selectBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	selectBox_->setInsertPolicy(QComboBox::NoInsert);
	selectBox_->setEditable(false);
	connect(selectBox_,SIGNAL(currentIndexChanged(int)),this,SLOT(selectedIndexChanged(int)));
	connect(selectBox_,SIGNAL(editTextChanged(const QString&)),this,SLOT(selectedItemTextChanged(const QString&)));

	deleteAnalysisAction_ = new QAction(QIcon(":/icons/delete.png"),
                               tr("Delete Analysis"), this);
    deleteAnalysisAction_->setStatusTip(tr("Delete Analysis"));
	deleteAnalysisAction_->setEnabled(false);
	connect(deleteAnalysisAction_,SIGNAL(triggered()),this,SLOT(deleteSelectedAnalysis()));
	QToolButton* deleteAnalysisButton = new QToolButton();
	deleteAnalysisButton->setDefaultAction(deleteAnalysisAction_);

	QHBoxLayout* layout(new QHBoxLayout());
	layout->addWidget(selectBox_);
	layout->addWidget(deleteAnalysisButton);
	setLayout(layout);
}

void AnalysisOptionWidget::setDesignRoot(QSharedPointer<DesignRoot> designRoot)
{
	if(designRoot_)
	{
		disconnect(designRoot_.data(),SIGNAL(refreshedFromXml()),this,SIGNAL(updateAnalysisList()));
	}
	designRoot_ = designRoot;
	if(designRoot_)
	{
		connect(designRoot_.data(),SIGNAL(refreshedFromXml()),this,SIGNAL(updateAnalysisList()));
	}
	updateAnalysisList();
}

void AnalysisOptionWidget::setSelectedAnalysis(QUuid analysisId)
{
	if(analysisId == QUuid())
	{
		selectBox_->setCurrentIndex(0);
		return;
	}
	for(int i=1;i<selectBox_->count()-1;i++)
	{
		if(selectBox_->itemData(i).toUuid() == analysisId)
		{
			selectBox_->setCurrentIndex(i);
			return;
		}
	}
}

void AnalysisOptionWidget::setSelectedAnalysis(QSharedPointer<Analysis> analysis)
{
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
	//Check if an update is needed
	bool needsUpdate = false;
	QUuid currAnalysisId;
	QString currName;
	for(int i=0;i<designRoot_->getNumAnalyses();i++)
	{
		currAnalysisId = designRoot_->getAnalysis(i).staticCast<Analysis>()->getAssociateId();
		currName = designRoot_->getAnalysis(i)->getName();
		if((selectBox_->itemData(i+1).toUuid() != currAnalysisId) 
			|| (selectBox_->itemText(i+1) != currName) )
		{
			needsUpdate = true;
			continue;
		}
	}
	if(selectBox_->count()-2 != designRoot_->getNumAnalyses())
		needsUpdate = true;

	if(!needsUpdate)
		return;

	updatingList_ = true;
	QUuid lastAnalysisId = selectBox_->itemData(selectBox_->currentIndex()).toUuid();
	int newSelectionIndex = 0;
	selectBox_->clear();
	selectBox_->addItem("None");
	for(int i=0;i<designRoot_->getNumAnalyses();i++)
	{
		currAnalysisId = designRoot_->getAnalysis(i).staticCast<Analysis>()->getAssociateId();
		selectBox_->addItem(designRoot_->getAnalysis(i)->getName(),currAnalysisId);
		if(!lastAnalysisId.isNull() && currAnalysisId == lastAnalysisId)
			newSelectionIndex = i+1;
	}
	selectBox_->addItem("New...");
	updatingList_ = false;
	selectBox_->setCurrentIndex(newSelectionIndex);
	selectedIndexChanged(newSelectionIndex);
}

void AnalysisOptionWidget::createNewAnalysis()
{
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
	selectBox_->setCurrentIndex(selectBox_->count()-2);
}

void AnalysisOptionWidget::selectedIndexChanged(int selectedIndex)
{
	//If the list is being updated, ignore changes
	if(updatingList_)
		return;

	Q_ASSERT(designRoot_);
	Q_ASSERT(selectedIndex <= designRoot_->getNumAnalyses()+1);
	QSharedPointer<Analysis> prevAnalysis = selectedAnalysis_;
	//If the option to create a new analysis was selected
	if(selectedIndex == designRoot_->getNumAnalyses()+1)
	{
		deleteAnalysisAction_->setEnabled(false);

		//Create a new analysis
		createNewAnalysis();
		return;
	}

	//If no analysis was selected
	if(selectedIndex <= 0)
	{
		deleteAnalysisAction_->setEnabled(false);

		//Erase the current selected analysis pointer
		selectedAnalysis_.clear();
	}
	else
	{
		deleteAnalysisAction_->setEnabled(true);

		//Update the current selected analysis pointer
		selectedAnalysis_ = designRoot_->getAnalysis(selectedIndex-1).staticCast<Analysis>();
	}

	if(prevAnalysis != selectedAnalysis_)
	{
		//Tell the editorState about the new current analysis
		editorState_->setCurrentAnalysis(selectedAnalysis_);

		//Since the selected analysis changed, refresh the UI so that everything will be updated with the
		//new current analysis
		editorState_->triggerExperimentReset();
	}

}

void AnalysisOptionWidget::selectedItemTextChanged(const QString& selectedItemText)
{
	int currIndex = selectBox_->currentIndex();
	Q_ASSERT(currIndex >= 0 && currIndex <= designRoot_->getNumAnalyses()+1);
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

}