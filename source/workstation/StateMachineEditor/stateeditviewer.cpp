#include <QtWidgets>
#include <QLabel>
#include <QMenuBar>
#include <QDialog>

#include "StateEditViewer.h"
#include "../../common/memleakdetect.h"
/*! \brief The interval at which automatic backups are saved.*/
#define AUTOSAVEINTERVALMS 30000	//auto save every 30 seconds

StateEditViewer::StateEditViewer(QWidget *parent) :
	Viewer(parent),
	expDesigner_(new Designer()),
	autoSaver_(AutoSaver::getSingleton()),
	checkedAutoSave_(false)
{
	connect(autoSaver_.data(),SIGNAL(openDesign(QSharedPointer<DesignRoot>)),this,SIGNAL(loadDesignRoot(QSharedPointer<DesignRoot>)));
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(expDesigner_);
	setLayout(mainLayout);
}

StateEditViewer::~StateEditViewer()
{
}
/*! \brief Called just before displaying the viewer.
 *	\details Moves the DesignRoot out of run mode so that Property changes won't
 *	only go into Property run values.  Loads the DesignRoot into the Designer widget
 *	object and activates that widget.  Starts the Autosaver so that changes made to
 *	the DesignRoot will be backed up in case of a crash.  In case we just
 *	restarted after a crash, the AutoSaver's DesignRestoreDialog is shown here as well.
 */
void StateEditViewer::init()
{
	designRoot_->enableRunMode(false);
	expDesigner_->activate(true);
	expDesigner_->loadDesign(designRoot_);
	autoSaver_->start(AUTOSAVEINTERVALMS);

	if(!checkedAutoSave_)
	{
		checkedAutoSave_ = true;		
		QDialog* autoSaveDialog = autoSaver_->getDesignRestoreDialog(this);
		if(autoSaveDialog)
		{
			autoSaveDialog->exec();
		}
	}
}

/*! \brief Called just before hiding the viewer.
 *	\details Sets an undo point, deactivates the Designer widget, stops
 *	the Autosave system and emits the deinitComplete() signal so that the 
 *	next Viewer can be displayed.
 */
void StateEditViewer::deinit()
{
	//Whenever we're leaving the StateEditViewer, set an undo point.  This way we will
	//be sure that the text stored in the design root matches the current state of the
	//software design
	designRoot_->setUndoPoint();
	expDesigner_->activate(false);
	autoSaver_->stop();
	emit deinitComplete();
}

void StateEditViewer::aboutToSave()
{
}