#include <QSlider>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include "OutputWidgetHolder.h"
#include "../../common/parameter/AnalysisOutput.h"
#include "../../common/memleakdetect.h"
#include "SaveOutputDialog.h"


OutputWidgetHolder::OutputWidgetHolder(QWidget *parent) :
QFrame(parent)
{
	resetLayout();
	containerWidget_ = NULL;
	updateTimer_ = new QTimer(this);
	connect(updateTimer_,SIGNAL(timeout()),this,SLOT(update()));
}

OutputWidgetHolder::~OutputWidgetHolder()
{
}

/*! \brief Called when a new run is started with the input run Id.
 *	\details Resets the Widget to display the Analysis Output for the new Run.
 */
void OutputWidgetHolder::newRunStarted(QUuid runId)
{
	if(runId != latestRunId_)
	{
		//Reset the layout to its initial condition
		resetLayout();
		//Update latest run id
		latestRunId_ = runId;
		//Since (due to threading) widgets may not be available immediately after run start, call update every 500ms until they are.
		updateTimer_->start(500);
	}
}

/*! \brief Sets the parameters that will be used when saving the underlying AnalysisOutputWidget data to file.
 *	@param saveToPath The file path at which data will be saved.
 *	@param separateSubDirs If true, the output files will go into their own directory named according to the
 *		Run name in the saveToPath directory.  If false, the output files will go directly into the saveToPath
 *		directory.
 */
void OutputWidgetHolder::setSaveParameters(QString saveToPath,bool separateSubDirs)
{
	saveToPath_ = saveToPath;
	separateSubDirs_ = separateSubDirs;
}

/*! \brief Uses OutputWidgetContainer::saveOutputTo() to save the AnalysisOutputWidget data to disk using
 *	the parameters defined in setSaveParameters().
*/
void OutputWidgetHolder::saveOutput()
{
	if(!containerWidget_ || saveToPath_.isEmpty())
		return;
	containerWidget_->saveOutputTo(saveToPath_,separateSubDirs_);
}

/*! \brief This gets called periodically until an OutputWidgetContainer is found for the Experiment Analysis.  When it is
 *	it is added to this widget's layout.
 *	\details The OutputWidgetContainer may not be immediately available after run start due to threading issues.  This is
 *	why we need to use this periodic update system.
 */
void OutputWidgetHolder::update()
{
	containerWidget_ = Picto::AnalysisOutput::getContainerWidget(latestRunId_);
	if(containerWidget_)
	{
		layout()->addWidget(containerWidget_);
		if(containerWidget_->isSaveable())
			saveButton_->setEnabled(true);
		else
			connect(containerWidget_,SIGNAL(detectedSaveable()),this,SLOT(enableSaveButton()));
		updateTimer_->stop();
	}
}

/*! \brief Clears this widget's layout, removing the OutputWidgetContainer.  This is used whenever a new Run starts
 *	so that the widget can display that AnalysisOutputWidget object of the new Run.
 */
void OutputWidgetHolder::resetLayout()
{
	//Delete last container widget layout and children (by putting it on another widget and deleting it)
	containerWidget_ = NULL;
	if(layout())
		QWidget().setLayout(layout());
	//Rebuild the layout
	QHBoxLayout* topLayout(new QHBoxLayout());
	saveButton_ = new QPushButton(QIcon("://icons/filesave.png"),"");
	saveButton_->setToolTip("Save Analysis Output");
	saveButton_->setEnabled(false);
	connect(saveButton_,SIGNAL(released()),this,SLOT(saveOutputFromDialog()));
	topLayout->addWidget(saveButton_,0,Qt::AlignRight | Qt::AlignTop);
	QVBoxLayout* layout(new QVBoxLayout());
	layout->addLayout(topLayout);
	setLayout(layout);
}

/*! \brief Called when the save button is pressed to retrieve save parameters from a SaveOutputDialog amd save
 *	all AnalysisOutputWidget data for this Experiment to disk.
 *	\details Uses setSaveParameters() and saveOutput() functions.
 */
void OutputWidgetHolder::saveOutputFromDialog()
{
	//Restore dialog values
	QString dirName = ".";
	bool useSeperateSubDirs = true;
	SaveOutputDialog saveDialog(this,dirName,useSeperateSubDirs);
	saveDialog.showDialog();
	dirName = saveDialog.getSelectedDir();
	if(dirName.isEmpty())
		return;
	useSeperateSubDirs = saveDialog.useSeperateSubDirs();
	setSaveParameters(dirName,useSeperateSubDirs);
	saveOutput();
}

/*! \brief Called when OutputWidgetContainer::detectedSaveable() is emitted.  Enables this widget's save button so that
 *	AnalysisOutputWidget data can be saved to disk.
 */
void OutputWidgetHolder::enableSaveButton()
{
	if(saveButton_)
		saveButton_->setEnabled(true);
}