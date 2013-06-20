#include "mainwindow.h"
#include "../common/globals.h"

#include <QtWidgets>

#include "errorlist.h"
#include "viewer.h"
//#include "texteditor/textviewer.h"
#include "testviewer/testviewer.h"
#include "remoteviewer/remoteviewer.h"
#include "replayviewer/replayviewer.h"
#include "statemachineeditor/stateeditviewer.h"
//#include "analysis/analysisviewer.h"
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

//! Creates all of the actions used by the main window
void MainWindow::createActions()
{
	//FileActions
	//--------------------------------------
	newExperimentAction_ = new QAction(tr("&New"), this);
	newExperimentAction_->setIcon(QIcon(":/icons/filenew.png"));
	newExperimentAction_->setShortcut(QKeySequence::New);
	newExperimentAction_->setStatusTip(tr("Create a new experiment"));
	connect(newExperimentAction_,SIGNAL(triggered()),this, SLOT(newExperiment()));

	openExperimentAction_ = new QAction(tr("&Open"), this);
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

	//Edit actions
	//--------------------------------------
	/*cutAction_ = new QAction(tr("Cut&"), this);
	cutAction_->setIcon(QIcon(":/icons/cut.png"));
	cutAction_->setShortcut(QKeySequence::Cut);
	cutAction_->setEnabled(false);
	connect(cutAction_,SIGNAL(triggered()),currViewer_, SLOT(cut()));
	connect(currViewer_,SIGNAL(enableCutAction(bool)),cutAction_,SLOT(setEnabled(bool)));

	copyAction_ = new QAction(tr("&Copy"), this);
	copyAction_->setIcon(QIcon(":/icons/copy.png"));
	copyAction_->setShortcut(QKeySequence::Copy);
	copyAction_->setEnabled(false);
	connect(copyAction_,SIGNAL(triggered()),currViewer_, SLOT(copy()));
	connect(currViewer_,SIGNAL(enableCopyAction(bool)),copyAction_,SLOT(setEnabled(bool)));

	pasteAction_ = new QAction(tr("Cut&"), this);
	pasteAction_->setIcon(QIcon(":/icons/paste.png"));
	pasteAction_->setShortcut(QKeySequence::Paste);
	pasteAction_->setEnabled(false);
	connect(cutAction_,SIGNAL(triggered()),currViewer_, SLOT(paste()));
	connect(currViewer_,SIGNAL(enablePasteAction(bool)),pasteAction_,SLOT(setEnabled(bool)));*/

	//Experiment actions
	//--------------------------------------
	//checkSyntaxAction_ = new QAction(tr("&Check XML syntax"),this);
	//checkSyntaxAction_->setShortcut(Qt::Key_F7);
	//checkSyntaxAction_->setToolTip(tr("Check the current experiment's XML code for syntax errors."));
	//checkSyntaxAction_->setIcon(QIcon(":/icons/checksyntax.png"));
	//connect(checkSyntaxAction_, SIGNAL(triggered()), this, SLOT(checkSyntax()));

	aboutPictoAction_ = new QAction(tr("&About Picto"),this);
	connect(aboutPictoAction_, SIGNAL(triggered()), this, SLOT(aboutPicto()));
}

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

	/*editMenu_ = menuBar()->addMenu(tr("&Edit"));
	editMenu_->addAction(cutAction_);
	editMenu_->addAction(copyAction_);
	editMenu_->addAction(pasteAction_);*/

	modeMenu_ = menuBar()->addMenu(tr("&Mode"));

	aboutMenu_ = menuBar()->addMenu(tr("&About"));
	aboutMenu_->addAction(aboutPictoAction_);
}

void MainWindow::createToolbars()
{
	fileToolbar_ = addToolBar(tr("&File"));
	fileToolbar_->addAction(newExperimentAction_);
	fileToolbar_->addAction(openExperimentAction_);
	fileToolbar_->addAction(saveExperimentAction_);

	/*editToolbar_ = addToolBar(tr("&Edit"));
	editToolbar_->addAction(cutAction_);
	editToolbar_->addAction(copyAction_);
	editToolbar_->addAction(pasteAction_);*/

	viewerToolbar_ = new QToolBar(tr("&Mode"),this);
	addToolBar(Qt::LeftToolBarArea,viewerToolbar_);
	viewerToolbar_->setMovable(false);
	viewerToolbar_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	viewerToolbar_->setIconSize(QSize(64,64));
	viewerToolbar_->setStyleSheet("QToolBar { background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop: 0 darkGreen stop: 0.5 white stop: 1 darkGreen);}");

}

/*!	\brief Creates all of the different viewers
 *
 *	In order to save time, all of the different viewers are created at start-up.
 *	Then, the textViewer is set as the current viewer.
 *
 *  To keep all of the code in one place, this function handles all of the actions 
 *	that are added to the viewer toolbar as well as putting together the
 *	stacked widget that contains the different viewer widgets
 *
 *	\TODO Store the previous setup and restore it on startup.
 */
void MainWindow::createViewers()
{
	viewerStack_ = new QStackedWidget(this);

	QAction* viewerAction;
	
	Viewer* viewer;

	////Text Viewer
	//viewer = new TextViewer(checkSyntaxAction_,this);
	//viewerNames_.append(viewer->type());
	//viewerStack_->addWidget(viewer);
	//viewerAction = new QAction(tr("Te&xt Editor"),this);
	//viewerAction->setShortcut(tr("Ctrl+1"));
	//viewerAction->setIcon(QIcon(":/icons/texteditmode.png"));
	//viewerAction->setData(0);
	//viewerToolbar_->addAction(viewerAction);
	//modeMenu_->addAction(viewerAction);
	//connect(viewerAction, SIGNAL(triggered()), this, SLOT(changeMode()));

	//StateEditViewer
	//NOTE: This hasn't yet been created, so we're using a test viewer
	//as a placeholder
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

	////Analysis Viewer
	//viewer = new AnalysisViewer(this);
	//viewerNames_.append(viewer->type());
	//viewerStack_->addWidget(viewer);
	//viewerAction = new QAction(tr("&Analyze experiment"),this);
	//viewerAction->setShortcut(tr("Ctrl+6"));
	//viewerAction->setIcon(QIcon(":/icons/analyze.png"));
	//viewerAction->setData(viewerNames_.size()-1);
	//viewerToolbar_->addAction(viewerAction);
	//modeMenu_->addAction(viewerAction);
	//connect(viewerAction, SIGNAL(triggered()), this, SLOT(changeMode()));
	//connect(viewer, SIGNAL(deinitComplete()), this, SLOT(startMode()));

	//If an application update fails, we want to go back to the statemachineeditor.
	connect(UpdateDownloader::getInstance().data(),SIGNAL(updateFailed()),initViewerAction_,SLOT(trigger()));
}

/*****************************************************
 *
 *			SLOTS
 *
 *****************************************************/

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

void MainWindow::newExperiment()
{
	//if(!pictoData_)
	//{
	//	pictoData_ = QSharedPointer<Picto::PictoData>(Picto::PictoData::Create().staticCast<PictoData>());
	//}
	if(okToContinue())
	{
		loadFile(DEFAULT_FILE);
		//pictoData_->clear();
		//pictoDataText_.clear();

		//QString pictoDataXml;
		//QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&pictoDataXml));
		//xmlWriter->setAutoFormatting(true);
		//xmlWriter->setAutoFormattingIndent(-1);   //use 1 tab to indent
		//xmlWriter->writeStartDocument();

		//pictoData_->toXml(xmlWriter);
		//pictoDataText_.setPlainText(pictoDataXml);
		//pictoDataText_.setModified(false);

		//setCurrentFile("");

		//for(int i=0; i<viewerStack_->count(); i++)
		//{
		//	Viewer *viewer = qobject_cast<Viewer*>(viewerStack_->widget(i));
		//	viewer->setPictoData(pictoData_);
		//	viewer->setPictoDataText(&pictoDataText_);
		//}
		//currViewer_->init();
		setCurrentFile("");
	}

}

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

void MainWindow::openSession()
{
	if(okToContinue())
	{
		QString filename = QFileDialog::getOpenFileName(this,
				tr("Open Experiment"),".","XML files (*.xml)");
		if(!filename.isEmpty())
			loadFile(filename);
	}
}

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

//! Called to save a file
bool MainWindow::saveExperiment()
{
	if(currFile_.isEmpty() || currFile_.right(4).toLower() != ".xml")
		return saveAsExperiment();
	else
		return saveFile(currFile_);
}

bool MainWindow::saveAsExperiment()
{
	//QFileDialog fileDialog(this,
	//	tr("Save Experiment"),currFile_.isEmpty()?".":currFile_,"XML files (*.xml)");
	//fileDialog.setFileMode(QFileDialog::AnyFile);
	//fileDialog.setDefaultSuffix(".xml");
	//fileDialog.setAcceptMode(QFileDialog::AcceptSave);
	//fileDialog.exec();
	//if(fileDialog.selectedFiles().isEmpty())
	//	return false;
	//QString filename = fileDialog.selectedFiles().first();
	//if(filename.isEmpty())
	//	return false;
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

//! Checks the syntax of the current XML to see if it is a legal experiment
//void MainWindow::checkSyntax()
//{
//	errorList_->clear();
//	if(designRoot_->compiles())
//	{
//		QMessageBox box;
//		box.setText("Syntax check passed");
//		box.setIconPixmap(QPixmap(":/icons/check.png"));
//		box.exec();
//	}
//	else
//	{
//		QString allErrors;
//		foreach(QString str, errorList_->getAllErrors())
//		{
//			allErrors.append(str);
//		}
//
//		QMessageBox box;
//		box.setText("Syntax check failed                                       ");
//		box.setDetailedText(errorList_->getAllErrors().join("\n"));
//		box.setIconPixmap(QPixmap(":/icons/x.png"));
//		box.exec();
//	}
//}

void MainWindow::aboutPicto()
{
	QStringList releaseNoteList;
	//List release notes
	releaseNoteList.append("Fixed bug that caused analysis crash when using AnalysisListVariables.");
	releaseNoteList.append("Fixed intermittent bug causing crash when attempting to create a new state transition while an old one was selected.");
	releaseNoteList.append("Fixed bug in searching experimental elements in levels above the current window.");

	releaseNoteList.append("Added Analysis Design Framework to State Edit Viewer.");
	releaseNoteList.append("Add Analysis execution and Analysis Widget display to Test Viewer.");	
	releaseNoteList.append("Add Analysis execution and Analysis Widget display to Replay Viewer.");	

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
//! Checks to see if we have unsaved changes
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


//! Does the actual file saving
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
		setCurrentFile(filename);
		changeMode(currViewer_);
		designRoot_->setUnmodified();
		return true;
	}
}

//! Does the actual file loading
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

	if(filename == DEFAULT_FILE)
		setCurrentFile("");
	else
		setCurrentFile(filename);

	return true;
}

//! Called whenever the current file is changed.
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
	//bool legalPictoDataXml = convertTextToPictoData();

	//for(int i=0; i<viewerStack_->count(); i++)
	//{
	//	Viewer *viewer = qobject_cast<Viewer*>(viewerStack_->widget(i));

	//	//if(legalPictoDataXml)
	//	//	viewer->setPictoData(pictoData_);
	//	//else
	//	//	viewer->setPictoData(QSharedPointer<Picto::PictoData>());
	//	viewer->setDesignRoot(designRoot_);
	//	
	//	//viewer->setPictoDataText(&pictoDataText_);
	//}
	if(currViewer_)
		changeMode(currViewer_);
	else
		initViewerAction_->trigger();
}

///*!	\brief attempts to convert the experiment text to an experiment object
// *
// *	If the experiment text can't be converted to an object (if the XML is
// *	incorrect, this function emits an error signal containing the text of 
// *	the XML parsing error, and returns false.
// */
//bool MainWindow::convertTextToPictoData()
//{
//	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(pictoDataText_.toPlainText()));
//
//	//read until we either see an experiment tag, or the end of the file
//	while(xmlReader->name() != "PictoData" && !xmlReader->atEnd()) 
//		xmlReader->readNext();
//
//	if(xmlReader->atEnd())
//	{
//		emit error("XML Parsing Error","Picto Data XML did not contain <PictoData> tag");
//		return false;
//	}
//
//	pictoData_ = QSharedPointer<Picto::PictoData>(Picto::PictoData::Create().staticCast<PictoData>());
//	Picto::Asset::clearErrors();
//	bool result = pictoData_->fromXml(xmlReader);
//
//	////!!!!!!!!!!!!!!!!!THIS IS FOR TESTING ONLY.  ITS A TOTAL WASTE OF TIME. REMOVE IT!!!!!!!
//	//QString serialTestString;
//	//QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&serialTestString));
//	//pictoData_->toXml(xmlWriter);
//	//Q_ASSERT(serialTestString == pictoDataText_.toPlainText());
//	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//	if(!result)
//	{
//		emit error("XML Parsing Error",pictoData_->getErrors());
//		return false;
//	}
//	return true;
//}

//! Modifies the recent files portion of the File menu
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
/*! \Brief stores settings
 *
 *	Settings for the workstation app are stored between sessions.
 *	The QSettings object does this for us in a platform independent
 *	manner.  This uses the registry in Windows, XML preference files
 *	in OSX, and ini files in Unix.
 */
void MainWindow::writeSettings()
{
	QSettings settings("Block Designs", Picto::Names->workstationAppName);

	settings.setValue("recentFiles",recentFiles_);

}

void MainWindow::readSettings()
{
	QSettings settings("Block Designs", Picto::Names->workstationAppName);

	recentFiles_= settings.value("recentFiles").toStringList();
	updateRecentFileActions();
}