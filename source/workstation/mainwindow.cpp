#include "mainwindow.h"
#include "../common/globals.h"

#include <QtWidgets>

#include "errorlist.h"
#include "viewer.h"
#include "testviewer/testviewer.h"
#include "remoteviewer/remoteviewer.h"
#include "replayviewer/replayviewer.h"
#include "statemachineeditor/stateeditviewer.h"
#include "../common/update/updatedownloader.h"
#include "../common/memleakdetect.h"

#define DEFAULT_FILE ":/BuiltInExperiments/EyeCalibration.xml"

MainWindow::MainWindow()
{	
	designRoot_ = QSharedPointer<DesignRoot>(new DesignRoot());
	currViewer_ = NULL;
	nextViewer_ = NULL;
	errorList_ = new ErrorList(this);
	connect(this, SIGNAL(error(QString, QString)), errorList_,SLOT(addError(QString, QString)));

	createActions();
	createMenus();
	createToolbars();
	readSettings();

	createViewers();

	setCentralWidget(viewerStack_);
	setWindowIcon(QIcon(":/icons/scope.ico"));

	isModified_ = false;
	//newExperiment();
	if(recentExperimentsActions_[0]->isVisible())
		recentExperimentsActions_[0]->trigger();
	else
		newExperiment();
}

/*****************************************************
 *
 *			SETUP FUNCTIONS
 *
 *****************************************************/

/*! \brief Creates all of the actions used by the MainWindow.
 */
void MainWindow::createActions()
{
	//FileActions
	//--------------------------------------
	newExperimentAction_ = new QAction(tr("&New"), this);
	newExperimentAction_->setIcon(QIcon(":/icons/filenew.png"));
	newExperimentAction_->setShortcut(QKeySequence::New);
	newExperimentAction_->setStatusTip(tr("Create a new experiment"));
	connect(newExperimentAction_,SIGNAL(triggered()),this, SLOT(newExperiment()));

	openExperimentAction_ = new QAction(tr("&Open Design"), this);
	openExperimentAction_->setIcon(QIcon(":/icons/fileopen.png"));
	openExperimentAction_->setShortcut(QKeySequence::Open);
	openExperimentAction_->setStatusTip(tr("Open an experiment"));
	connect(openExperimentAction_,SIGNAL(triggered()),this, SLOT(openExperiment()));

	saveExperimentAction_ = new QAction(tr("&Save"), this);
	saveExperimentAction_->setIcon(QIcon(":/icons/filesave.png"));
	saveExperimentAction_->setShortcut(QKeySequence::Save);
	saveExperimentAction_->setStatusTip(tr("Save this experiment"));
	connect(saveExperimentAction_,SIGNAL(triggered()),this, SLOT(saveExperiment()));

	saveAsExperimentAction_ = new QAction(tr("Save &As"), this);
	saveAsExperimentAction_->setShortcut(QKeySequence::SaveAs);
	saveAsExperimentAction_->setStatusTip(tr("Save this experiment with a different filename"));
	connect(saveAsExperimentAction_,SIGNAL(triggered()),this, SLOT(saveAsExperiment()));

	for(int i=0; i<MaxRecentFiles; i++)
	{
		recentExperimentsActions_[i] = new QAction(this);
		recentExperimentsActions_[i]->setVisible(false);
		connect(recentExperimentsActions_[i], SIGNAL(triggered()), this, SLOT(openRecentExperiment()));
	}

	exitAction_ = new QAction(tr("&Quit"),this);
	exitAction_->setShortcut(tr("Ctrl+Q"));
	exitAction_->setStatusTip(tr("Exit")+Picto::Names->workstationAppName);
	connect(exitAction_, SIGNAL(triggered()), this, SLOT(close()));

	setSystemNumberAction_ = new QAction(tr("&Change System Number"),this);
	connect(setSystemNumberAction_,SIGNAL(triggered()),this,SLOT(changeSystemNumber()));

	aboutPictoAction_ = new QAction(tr("&About Picto"),this);
	connect(aboutPictoAction_, SIGNAL(triggered()), this, SLOT(aboutPicto()));

}

/*! \brief Creates all of the menus used by the MainWindow.
 */
void MainWindow::createMenus()
{
	fileMenu_ = menuBar()->addMenu(tr("&File"));
	fileMenu_->addAction(newExperimentAction_);
	fileMenu_->addAction(openExperimentAction_);
	fileMenu_->addAction(saveExperimentAction_);
	fileMenu_->addAction(saveAsExperimentAction_);

	seperatorAction_ = fileMenu_->addSeparator();
	for(int i=0; i<MaxRecentFiles; i++)
		fileMenu_->addAction(recentExperimentsActions_[i]);
	fileMenu_->addSeparator();
	fileMenu_->addAction(exitAction_);

	modeMenu_ = menuBar()->addMenu(tr("&Mode"));
	systemMenu_ = menuBar()->addMenu(tr("S&ystem"));
	systemMenu_->addAction(setSystemNumberAction_);
	aboutMenu_ = menuBar()->addMenu(tr("&About"));
	aboutMenu_->addAction(aboutPictoAction_);
}

/*! \brief Creates all of the toolbars used by the MainWindow.
 */
void MainWindow::createToolbars()
{
	fileToolbar_ = addToolBar(tr("&File"));
	fileToolbar_->addAction(newExperimentAction_);
	fileToolbar_->addAction(openExperimentAction_);
	fileToolbar_->addAction(saveExperimentAction_);
	fileToolbar_->addSeparator();
	fileToolbar_->addWidget(new QLabel(QString("<h3>System Number: <span style=\"color:darkgreen\">%1</span></h3>").arg(Picto::portNums->getSystemNumber())));

	viewerToolbar_ = new QToolBar(tr("&Mode"),this);
	addToolBar(Qt::LeftToolBarArea,viewerToolbar_);
	viewerToolbar_->setMovable(false);
	viewerToolbar_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	viewerToolbar_->setIconSize(QSize(64,64));
	viewerToolbar_->setStyleSheet("QToolBar { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop: 0 darkGreen stop: 0.5 white stop: 1 darkGreen);}");

}

/*!	\brief Creates all of the different viewers.
 *
 *	\details In order to save time, all of the different viewers are created at start-up.
 *	The StateEditViewer is set as the current viewer.
 *
 *  To keep all of the code in one place, this function handles all of the actions 
 *	that are added to the viewer toolbar for selecting a different viewer as well as putting together the
 *	QStackedWidget that contains the different Viewer widgets.
 *	\note It would be nice if the system started at the same viewer that was being viewed before the last exit.
 *	We should implement that.
 */
void MainWindow::createViewers()
{
	viewerStack_ = new QStackedWidget(this);

	QAction* viewerAction;
	
	Viewer* viewer;

	//StateEditViewer
	viewer = new StateEditViewer(this);
	viewerNames_.append(viewer->type());
	viewerStack_->addWidget(viewer);
	viewerAction = new QAction(tr("&State Machine Editor"),this);
	viewerAction->setShortcut(tr("Ctrl+2"));
	viewerAction->setIcon(QIcon(":/icons/statemachineeditmode.png"));
	viewerAction->setData(viewerNames_.size()-1);
	viewerToolbar_->addAction(viewerAction);
	modeMenu_->addAction(viewerAction);
	connect(viewerAction, SIGNAL(triggered()), this, SLOT(changeMode()));
	connect(viewer, SIGNAL(deinitComplete()), this, SLOT(startMode()));
	connect(static_cast<StateEditViewer*>(viewer),SIGNAL(loadDesignRoot(QSharedPointer<DesignRoot>)),this,SLOT(openDesign(QSharedPointer<DesignRoot>)));
	initViewerAction_ = viewerAction;

	//Test Viewer
	viewer = new TestViewer(this);
	viewerNames_.append(viewer->type());
	viewerStack_->addWidget(viewer);
	viewerAction = new QAction(tr("&Test experiment"),this);
	viewerAction->setShortcut(tr("Ctrl+3"));
	viewerAction->setIcon(QIcon(":/icons/testmode.png"));
	viewerAction->setData(viewerNames_.size()-1);
	viewerToolbar_->addAction(viewerAction);
	modeMenu_->addAction(viewerAction);
	connect(viewerAction, SIGNAL(triggered()), this, SLOT(changeMode()));
	connect(viewer, SIGNAL(deinitComplete()), this, SLOT(startMode()));

	//remote viewer
	viewer = new RemoteViewer(this);
	viewerNames_.append(viewer->type());
	viewerStack_->addWidget(viewer);
	viewerAction = new QAction(tr("&Run remote experiment"),this);
	viewerAction->setShortcut(tr("Ctrl+4"));
	viewerAction->setIcon(QIcon(":/icons/remote.png"));
	viewerAction->setData(viewerNames_.size()-1);
	viewerToolbar_->addAction(viewerAction);
	modeMenu_->addAction(viewerAction);
	connect(viewerAction, SIGNAL(triggered()), this, SLOT(changeMode()));
	connect(viewer, SIGNAL(deinitComplete()), this, SLOT(startMode()));

	//Replay viewer
	viewer = new ReplayViewer(this);
	viewerNames_.append(viewer->type());
	viewerStack_->addWidget(viewer);
	viewerAction = new QAction(tr("&Replay experiment"),this);
	viewerAction->setShortcut(tr("Ctrl+5"));
	viewerAction->setIcon(QIcon(":/icons/replay.png"));
	viewerAction->setData(viewerNames_.size()-1);
	viewerToolbar_->addAction(viewerAction);
	modeMenu_->addAction(viewerAction);
	connect(viewerAction, SIGNAL(triggered()), this, SLOT(changeMode()));
	connect(viewer, SIGNAL(deinitComplete()), this, SLOT(startMode()));

	//If an application update fails, we want to go back to the statemachineeditor.
	connect(UpdateDownloader::getInstance().data(),SIGNAL(updateFailed()),initViewerAction_,SLOT(trigger()));
}

/*****************************************************
 *
 *			SLOTS
 *
 *****************************************************/
/*! \brief Called when the MainWindow is told to close.  Calls aboutToQuit() on all of the viewers.  If nothing objects, the latest
 *	settings are written to disk and the close can proceed.
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
	//We're going to call the aboutToQuit slot on all of the viewer widgets
	//WARNING: Iterating through the stack like this only works if widgets are
	//inserted without leaving gaps in the indices.  This would seem to be a
	//weakness in Qt.
	for(int i=0; i<viewerStack_->count(); i++)
	{
		Viewer* v = qobject_cast<Viewer*>(viewerStack_->widget(i));
		if(v)
			if(!v->aboutToQuit())
			{
				event->ignore();
				return;
			}
	}

	if(okToContinue())
	{
		writeSettings();
		event->accept();
	}
	else
	{
		event->ignore();
	}
}

/*! \brief Called when the newExperimentAction_ is triggered.  If nothing objects to closing the current Design, a default
 *	Design is loaded (loadFile() with the EyeCalibration.xml Design).
 *	\note The naming here is from when the design xml only included an Experiment (no Analysis or UI data).  This name should
 *	probably be updated.
 */
void MainWindow::newExperiment()
{
	if(okToContinue())
	{
		loadFile(DEFAULT_FILE);
		setCurrentFile("");
	}

}

/*! \brief Called when the openExperimentAction_ is triggered.  If nothing objects to closing the current Design, a QFileDialog allows
 *	the user to choose a Design file and (if valid) that file is loaded.
 *	\note The naming here is from when the design xml only included an Experiment (no Analysis or UI data).  This name should
 *	probably be updated.
 */
void MainWindow::openExperiment()
{
	if(okToContinue())
	{
		QString filename = QFileDialog::getOpenFileName(this,
				tr("Open Experiment"),".","Design or Session files (*.xml *.sqlite)");
		if(!filename.isEmpty())
			loadFile(filename);
	}
}

/*! \brief Called when one of the recentExperimentActions_ is triggered.  If nothing objects to closing the current Design, the file
 *	referenced from the RecentExperimentAction that called this function is loaded.  If that fails, newExperiment() is called to load
 *	the default Design.
 *	\note The naming here is from when the design xml only included an Experiment (no Analysis or UI data).  This name should
 *	probably be updated.
 */
void MainWindow::openRecentExperiment()
{
	if(okToContinue())
	{
		QAction *action = qobject_cast<QAction*>(sender());
		if(action)
			if(!loadFile(action->data().toString()))
				newExperiment();
	}
}

/*! \brief This is called when the StateEditViewer requests that the input DesignRoot be loaded.  If nothing objects to closing the current Design,
 *	the input DesignRoot is loaded.
 *	\details Essentially, this is the same as openExperiment() except that it is opening a Design from RAM not a file, so it needs to take care of
 *	things differently and handle some of the complicating factors of this procedure.
 */
void MainWindow::openDesign(QSharedPointer<DesignRoot> designRoot)
{
	if(designRoot.isNull())
		return;
	if(!okToContinue())
		return;
	designRoot_ = designRoot;
	//Load the design root into the autosaver.  Auto save the initial version
	//since we are opening it from a RAM object that isn't saved anywhere
	AutoSaver::getSingleton()->setDesignRoot(designRoot_,true);

	//When we save this file, we need to force a saveas, so we set the current
	//file to be empty
	setCurrentFile("");
	setWindowTitle(QString("%1[*] - %2").arg(designRoot_->getDesignName())
								   .arg(Picto::Names->workstationAppName));
}


/*! \brief Called to save the Design to file.  Redirects to saveFile() with the current file name in the default case, or saveAsExperiment() in the case
 *	where there is no file defined yet where the data should go.
 *	\note The naming here is from when the design xml only included an Experiment (no Analysis or UI data).  This name should
 *	probably be updated.
 */
bool MainWindow::saveExperiment()
{
	if(currFile_.isEmpty() || currFile_.right(4).toLower() != ".xml")
		return saveAsExperiment();
	else
		return saveFile(currFile_);
}

/*! \brief Called to save the Design to a new file.  Allows the user to select a file in a QFileDialog.  The file data is written to
 *	the selected file using saveFile() with that new filename.
 *	\note The naming here is from when the design xml only included an Experiment (no Analysis or UI data).  This name should
 *	probably be updated.
 */
bool MainWindow::saveAsExperiment()
{
	QString filename = QFileDialog::getSaveFileName(this,
		tr("Save Experiment"),currFile_.isEmpty()?".":currFile_,"XML files (*.xml)");
	if(filename.isEmpty())
		return false;
	if(filename.right(4).toLower() != ".xml")
	{
		if(filename.contains("."))
		{
			QMessageBox::warning(NULL,"Invalid Extension","Design files must be saved with a .xml extension.  Please try again.");
			return false;
		}
		filename = filename.append(".xml");
	}

	return saveFile(filename);
}

/*! \brief Called when one of the Viewer actions is triggered.  Redirects to changeMode() with a pointer to the Viewer
 *	that we should switch to.
 */
void MainWindow::changeMode()
{
	QAction *action = qobject_cast<QAction*>(sender());
	if(!action)
		return;

	int viewerIndex = action->data().toInt();

	//find and set the next viewer
	viewerStack_->setCurrentIndex(viewerIndex);
	changeMode(qobject_cast<Viewer*>(viewerStack_->currentWidget()));
}

/*! \brief Initiates a change in focus to the input Viewer.  If there is no current Viewer, startMode() is used.  If there is a current Viewer, its
 *	deinit() function is called and startMode() will be called whien its deinitComplete() signal is triggered.
 *	\details For more information on why we use the deinitComplete() signal indirection to trigger startMode(), see Viewer::deinitComplete().
 */
void MainWindow::changeMode(Viewer* nextViewer)
{
	nextViewer_ = nextViewer;
	//Deinitialize the current viewer
	if(currViewer_)
	{
		Viewer* prevViewer = currViewer_;
		currViewer_ = NULL;
		prevViewer->deinit();	//The last viewer will tell us to start the next one when its ready
	}
	else
		startMode();	//Start the next viewer
}

/*! \brief Changes the current view mode to that of the nextViewer_.  Sets the current DesignRoot to that Viewer and calls its Viewer::init() function.
*/
void MainWindow::startMode()
{
	if(currViewer_ || !nextViewer_)
		return;
	currViewer_ = nextViewer_;
	//Set the latest experiment, experiment text, uidata, uidatatext to the viewer
	currViewer_->setDesignRoot(designRoot_);
	currViewer_->init();
	nextViewer_ = NULL;
}

/*! \brief Called when the setSystemNumberAction_ is triggered.  Allows the user to set the new system number and then uses Picto::portNums::setSystemNumber() to
 *	enact the change and restart the Workstation.
 */
void MainWindow::changeSystemNumber()
{
	int continueWithChange = QMessageBox::Cancel;
	continueWithChange = QMessageBox::warning(this,"Change System Number","You are about to change your workstation's system number.  This action will cause your workstation to restart.  If your workstation connects to a server with a newer version of Picto and you update, you will not be able to connect to your current system until its version is updated as well.\n\nContinue?",QMessageBox::Ok | QMessageBox::Cancel,QMessageBox::Cancel);
	if(continueWithChange != QMessageBox::Ok)
		return;
	if(!okToContinue())
		return;
	bool result = false;
	int newSystemNum = QInputDialog::getInt(this,"Change System Number","Select a new system number",Picto::portNums->getSystemNumber(),1,10,1,&result);
	if(!result)
		return;
	Picto::portNums->setSystemNumber(QCoreApplication::applicationFilePath(),QCoreApplication::arguments(),newSystemNum,true);
}

/*! \brief Called when the aboutPictoAction_ is triggered.  Displays a QMessageBox containing the latest release notes and general
 *	information about the Picto application.
 */
void MainWindow::aboutPicto()
{
	QStringList releaseNoteList;
	//List release notes
	releaseNoteList.append("Fixed bug that caused in session parameter value changes to be overridden by default values.");
	
	releaseNoteList.append("Fixed bug that caused property fields to be intermittently greyed out and read-only.");
	releaseNoteList.append("Overhaul of playback/analysis system to support batch analysis.");
	releaseNoteList.append("Added 'disable lfp' capability to speed up session playback/analysis when lfp data is not necessary.");
	releaseNoteList.append("Added auto-save system for restoring a recent unsaved version of a design file in the event of a crash.");

	//Format release notes:
	QString releaseNotes;
	if(releaseNoteList.size())
	{
		releaseNotes.append(QString("<h4 style=\"color:black\">Release Notes:</h4><ul>"));
		foreach(QString note,releaseNoteList)
		{
			releaseNotes.append(QString("<li><span style=\"color:black;\">%2</span></li>").arg(note));
		}
		releaseNotes.append("</ul>");
	}

	QMessageBox::about(this,"Picto Workstation",QString("<h4 style=\"color:black\">You are using version %1 of the Picto Workstation.</h4>%2").arg(PICTOVERSION).arg(releaseNotes));
}

/*****************************************************
 *
 *			FILE HANDLING
 *
 *****************************************************/
/*! \brief Checks to see if we have unsaved changes.  If so, uses a QMessageBox to ask the user if the unsaved changes are important or not.  If not, it is okay
 *	to continue and true is returned, if so, false is returned.
 */
bool MainWindow::okToContinue()
{
	if(designRoot_->isModified())
	{
		int r = QMessageBox::warning(this,Picto::Names->workstationAppName,
					tr("The experiment has been modified.\n"
					   "Do you want to save your changes?"),
					   QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
		if(r == QMessageBox::Yes)
			return saveExperiment();
		else if(r == QMessageBox::Cancel)
			return false;
	}
	return true;
}


/*! \brief Saves the current DesignRoot to the input file.
 *	\details The DesignRoot's name is changed to match that of the input filename, Viewer::aboutToSave() is called on the currentViewer, 
 *	the DesignRoot is serialized using DesignRoot::getDesignRootText(), and that text is saved to the file.
 */
bool MainWindow::saveFile(const QString filename)
{
	int success = true;
	QFile file(filename);
	if(file.open(QIODevice::WriteOnly))
	{
		//Make sure design name is the same as the filename
		designRoot_->setDesignName(QFileInfo(file).baseName());
		if(currViewer_)
			currViewer_->aboutToSave();
		if(!file.write(designRoot_->getDesignRootText().toLatin1()))
			success = false;
		file.close();
	}
	else
		success = false;

	if(!success)
	{
		QMessageBox::critical(this,Picto::Names->workstationAppName, 
			"Experiment failed to save");
		return false;
	}
	else
	{
		//The file saved succesfully

		//Tell the autosaver that it can get rid of the autosaved file cause there is now
		//a saved version.
		AutoSaver::getSingleton()->removeFileUntilNextChange();
		
		setCurrentFile(filename);
		changeMode(currViewer_);
		designRoot_->setUnmodified();
		return true;
	}
}

/*! \brief Loads the DesignRoot from the XML or .sqlite file at the input filename path.  
 *	\details This attemps to deserialize the file contents into a DesignRoot.  If deserialization fails, an error message is displayed and false is returned.
 *	If the deserialization is successful with warnings, those warnings are shown to the user.  Then the new DesignRoot is sent into the background AutoSaver 
 *	system for safety's sake in the case of a crash and the current filename is updated for the purpose of saveExperiment().
 */
bool MainWindow::loadFile(const QString filename)
{
	QFile file(filename);
	QSqlDatabase newSession;
	QString designXML = "";
	bool isDesignFile = filename.right(4).toLower() == ".xml";
	if(isDesignFile)
	{	//Its an xml design file
		if(!file.open(QIODevice::ReadOnly))
		{
			QMessageBox::critical(this, Picto::Names->workstationAppName,
				"Failed to open file: " + filename);
			return false;
		}

		designXML = file.readAll();
		file.close();
	}
	else
	{	//Its a sqlite session file
		QString path = filename;
		path = path.replace("\\","/");
		Q_ASSERT(path.lastIndexOf("/") >=0);
		Q_ASSERT(path.lastIndexOf(".") >=0);
		QString connectionName = path.mid(path.lastIndexOf("/")+1);
		connectionName = connectionName.left(connectionName.lastIndexOf("."));
		newSession = QSqlDatabase::addDatabase("QSQLITE",connectionName);
		newSession.setDatabaseName(path);
		newSession.open();
		if(!newSession.isOpen())
		{
			QSqlError error = newSession.lastError();
			QMessageBox::critical(0,"Could not open session",error.text());
			return false;
		}
		QSqlQuery query(newSession);

		//Get runs list.
		//ExperimentXML name changed to DesignXML when we started putting UI Data and Analysis Data together
		//with it on server.  They never appeared at the same time so we can just look for either one.
		query.prepare("SELECT value FROM sessioninfo WHERE key='ExperimentXML' OR key='DesignXML' LIMIT 1");
		bool success = query.exec();
		if(!success || !query.next())
		{
			QMessageBox::critical(0,"Failed to select Design XML from Session",success?"":query.lastError().text().toLatin1());
			return false;
		}
		designXML = query.value(0).toString();
		newSession.close();
	}
	if(designXML.isEmpty())
	{
		QMessageBox::critical(0,"Experiment design was empty.","Please try a different file");
		return false;
	}
	QSharedPointer<DesignRoot> newDesignRoot = QSharedPointer<DesignRoot>(new DesignRoot());
	if(!newDesignRoot->resetDesignRoot(designXML))
	{
	
		DesignMessage errorMsg = newDesignRoot->getLastError();
		QMessageBox box;
		box.setTextFormat(Qt::RichText);
		box.setText(errorMsg.name);
		box.setDetailedText(errorMsg.details);
		box.setIconPixmap(QPixmap(":/icons/x.png"));
		box.exec();
		return false;
	}
	if(newDesignRoot->hasWarning())
	{
		DesignMessage warnMsg = newDesignRoot->getLastWarning();
		QMessageBox::warning(0,warnMsg.name,warnMsg.details);
	}
	designRoot_ = newDesignRoot;
	//Load the design root into the autosaver.  Don't auto save the current version
	//because we just opened it from file, we already have a saved version.
	AutoSaver::getSingleton()->setDesignRoot(designRoot_,false);

	if(filename == DEFAULT_FILE)
		setCurrentFile("");
	else
		setCurrentFile(filename);

	return true;
}

/*! \brief Called whenever the current file is changed.  Updates various values accordingly.
 *	\details The currFile_ string used to find the file to use in saveExperiment() is updated.  The recent files list in the file dropdown is updated along with the
 *	window title.  Then changeMode() is called with the current Viewer in order to call Viewer::init() again.
 *	
 */
void MainWindow::setCurrentFile(const QString &filename)
{
	currFile_ = filename;
	isModified_ = false;

	QString shownName = tr("Untitled");
	if(!currFile_.isEmpty())
	{
		shownName = QFileInfo(currFile_).fileName();
		recentFiles_.removeAll(currFile_);
		recentFiles_.prepend(currFile_);
		updateRecentFileActions();
	}

	setWindowTitle(QString("%1[*] - %2").arg(shownName)
								   .arg(Picto::Names->workstationAppName));
	if(currViewer_)
		changeMode(currViewer_);
	else
		initViewerAction_->trigger();
}

/*! \brief Updates the recent files portion of the File menu according to the latest recentFiles_ list and a check as to which of those files actually exist
 *	in the file system.
 */
void MainWindow::updateRecentFileActions()
{
	//Clear out any non-existent files
	foreach(QString filename, recentFiles_)
	{
		if(!QFile::exists(filename))
			recentFiles_.removeAll(filename);
	}

	//truncate the list so that only the 5 most recent files are included
	recentFiles_ = recentFiles_.mid(0,5);

	//Set up an action for each recent file
	int actionIdx = 0;

	foreach(QString filename, recentFiles_)
	{
		QString actionText = QString("%1 %2").arg(actionIdx+1)
								.arg(QFileInfo(filename).fileName());
		recentExperimentsActions_[actionIdx]->setText(actionText);
		recentExperimentsActions_[actionIdx]->setVisible(true);
		recentExperimentsActions_[actionIdx]->setData(filename);
		actionIdx++;
	}
	
	//hide the leftover actions
	while(actionIdx++<MaxRecentFiles)
		recentExperimentsActions_[actionIdx]->setVisible(false);

	//Show/hide the seperator
	seperatorAction_->setVisible(!recentFiles_.isEmpty());
}

/*****************************************************
 *
 *			PERSISTANT SETTINGS
 *
 *****************************************************/
/*! \Brief Stores settings from the Workstation to disk.
 *
 *	\details Settings for the Workstation app are stored on disk.
 *	The QSettings object does this for us in a platform independent
 *	manner.  This uses the registry in Windows, XML preference files
 *	in OSX, and ini files in Unix.
 *
 *	Currently, the only settings are the recently opened files.
 */
void MainWindow::writeSettings()
{
	QSettings settings("Block Designs", Picto::Names->workstationAppName);

	settings.setValue("recentFiles",recentFiles_);

}

/*! \Brief Reads settings for the Workstation from disk.
 *
 *	\details Settings for the Workstation app are stored on disk.
 *	The QSettings object does this for us in a platform independent
 *	manner.  This uses the registry in Windows, XML preference files
 *	in OSX, and ini files in Unix.
 *
 *	Currently, the only settings are the recently opened files.
 */
void MainWindow::readSettings()
{
	QSettings settings("Block Designs", Picto::Names->workstationAppName);

	recentFiles_= settings.value("recentFiles").toStringList();
	updateRecentFileActions();
}