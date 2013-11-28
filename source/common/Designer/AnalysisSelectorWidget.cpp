#include <QCheckBox>
#include <QVBoxLayout>
#include <QLabel>
#include "AnalysisSelectorWidget.h"
#include "../../common/memleakdetect.h"
using namespace Picto;


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
/*! \brief Removes all local Analyses, displayed and not displayed, from this widget.*/
void AnalysisSelectorWidget::clearLocalAnalyses()
{
	localAnalysisLookup_.clear();
	currFilePath_ = "";
	updateLocalAnalysisList();
}

/*! \brief Adds the Analyses from the input DesignRoot to this widget for the input filePath.
 *	\details To show these Analyses in the widget, just call setCurrentFile() with the input filePath.
 */
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

/*! \brief Like setLocalDesignRoot, except the Analyses are added directly for the input filePath.
 *	\details In this function, the input analysisIds and analysisNames strings are matched up, line
 *	by line to build up the local analysis display in the widget.  Essentially, setLocalDesignRoot()
 *	is just extracting analysisIds and analysisNames automatically from its input DesignRoot and then
 *	just doing what we are doing here.  Storing that information in a fileName lookup table for
 *	display when setCurrentFIle() is called with the input fileName.
 */
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

/*! \brief Adds the Analyses from the input DesignRoot to this widget under the Import tab for possible import into another design file.
 */
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

/*! \brief Set the path of the design or session file current being used to display local Analyses for that file.
 *	\details The Analyses for individual filepaths are entered by using setLocalDesignRoot() or 
 * setLocalDesignAnalyses().
 */
void AnalysisSelectorWidget::setCurrentFile(QString filePath)
{
	currFilePath_ = filePath;
	updateLocalAnalysisList();
}

/*! \brief Returns a list of the Associate Ids for the widget's currently selected local analyses.
*/
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
/*! \brief Returns a list of the Associate Ids for the widget's currently selected importable analyses.
*/
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

/*! \brief Enables/Disables all checkboxes in this widget.  By default they are enabled.
*/
void AnalysisSelectorWidget::enableCheckboxes(bool enable)
{
	analysesBox_->setEnabled(enable);
	analysesForImportBox_->setEnabled(enable);
}

/*! \brief Returns true if something in the widget has been selected.*/
bool AnalysisSelectorWidget::hasSelectedIds()
{
	return bool(getSelectedAnalysisIdsForImport().size()) || bool(getSelectedAnalysisIds().size());
}

/*! \brief Updates the contents displayed in the local analysis tab based on the latest changes to the widget.
 *	\details This is called internally to update the widget when things like setCurrentFile() getting called happen.
*/
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

/*! \brief Updates the contents displayed in the import analysis tab based on the latest changes to the widget.
 *	\details This is called internally to update the widget when things like setDesignRootForImport() getting called happen.
 */
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

/*! \brief A slot that is emits the analysisWidgetChanged() signal whenever something happens to one of this widgets checkboxes.
*/
void AnalysisSelectorWidget::checkboxChanged(bool)
{
	emit analysisWidgetChanged();
}