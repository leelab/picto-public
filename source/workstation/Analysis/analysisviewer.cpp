//#include "AnalysisViewer.h"
//
//#include "../common/globals.h"
//
//
//
//#include <QToolBar>
//#include <QAction>
//#include <QMessageBox>
//#include <QVBoxLayout>
//#include <QHBoxLayout>
//#include <QStringList>
//#include <QComboBox>
//#include <QLabel>
//#include <QTextEdit>
//#include <QLineEdit>
//#include <QSharedPointer>
//#include <QMenu>
//#include <QCloseEvent>
//#include <QSqlQuery>
//#include <QSqlError>
//#include <QSqlRecord>
//#include <QFileDialog>
//#include <QSqlField>
//#include <QLinkedList>
//#include <QProgressBar>
//#include <QTabWidget>
//#include <QDir>
//
//#include "../../common/analysis/AnalysisOutputWidget.h"
//#include "SaveOutputDialog.h"
//#include "../../common/memleakdetect.h"
//using namespace Picto;
//
//
//AnalysisViewer::AnalysisViewer(QWidget *parent) :
//	Viewer(parent)
//{
//	setupUi();
//
//
//	QString dbName = "PictoWorkstation";
//	dbName = dbName.toLower();
//	configDb_ = QSqlDatabase::addDatabase("QSQLITE",dbName);
//	QString configPath = QCoreApplication::applicationDirPath()+"/../config";
//	QDir configDir(configPath);
//	if(!configDir.exists())
//	{
//		configDir.mkpath(configPath);
//		configDir = QDir(configPath);
//	}
//	configDb_.setDatabaseName(configDir.canonicalPath() + "/" + dbName + ".config");
//	configDb_.open();
//
//	QSqlQuery query(configDb_);
//	if(!configDb_.tables().contains("workstationinfo"))
//	{
//		query.exec("CREATE TABLE workstationinfo (key TEXT, value TEXT)");
//	}
//}
//
//AnalysisViewer::~AnalysisViewer()
//{
//}
////! Called just before displaying the viewer
//void AnalysisViewer::init()
//{
//	designRoot_->enableRunMode(false);
//	anaDesigner_->loadDesign("AnalysisContainer",0,designRoot_);
//}
//
////!Called just before hiding the viewer
//
//void AnalysisViewer::deinit()
//{
//	emit deinitComplete();
//}
//
//
//bool AnalysisViewer::aboutToQuit()
//{
//	return true;
//}
//
////! Sets up the user interface portions of the GUI
//void AnalysisViewer::setupUi()
//{
//	//
//	executeAction_ = new QAction(tr("&Start task"),this);
//	executeAction_->setIcon(QIcon(":/icons/play.png"));
//	connect(executeAction_,SIGNAL(triggered()),this, SLOT(executeCommand()));
//
//	loadSessionAction_ = new QAction(tr("&Load Session"),this);
//	connect(loadSessionAction_, SIGNAL(triggered()),this, SLOT(loadSession()));
//	loadSessionAction_->setEnabled(true);
//
//	currSessionLabel_ = new QLabel(tr("No Loaded Session"));
//
//	progressBar_ = new QProgressBar();
//	progressBar_->setRange(0,100);
//	progressBar_->reset();
//
//	progressBarTimer_ = new QTimer(this);
//	progressBarTimer_->setInterval(100);
//	progressBarTimer_->stop();
//	connect(progressBarTimer_,SIGNAL(timeout()),this,SLOT(updateProgressBar()));
//
//
//	saveOutputAction_ = new QAction(tr("&Save Output"),this);
//	connect(saveOutputAction_, SIGNAL(triggered()),this, SLOT(saveOutput()));
//	saveOutputAction_->setEnabled(false);
//	
//	outputDisplay_ = new AnalysisOutputDisplay();
//
//
//	anaDesigner_ = new Designer();
//
//	runSelector_ = new TaskRunSelector();
//
//	//The toolbar
//	toolBar_ = new QToolBar;
//
//	toolBar_->addAction(executeAction_);
//	toolBar_->addSeparator();
//	toolBar_->addAction(loadSessionAction_);
//	toolBar_->addSeparator();
//	toolBar_->addAction(saveOutputAction_);
//	toolBar_->addSeparator();
//	toolBar_->addWidget(currSessionLabel_);
//
//	QHBoxLayout *toolbarLayout = new QHBoxLayout;
//	toolbarLayout->addWidget(toolBar_);
//	toolbarLayout->addStretch();
//	
//
//	//------ Main layout -----------
//	mainTabWindow_ = new QTabWidget();
//	QVBoxLayout *runDefinitionLayout = new QVBoxLayout;
//	runDefinitionLayout->addWidget(runSelector_);
//	QWidget* runDefWidget = new QWidget();
//	runDefWidget->setLayout(runDefinitionLayout);
//	mainTabWindow_->addTab(anaDesigner_,"Analysis Designer");
//	mainTabWindow_->addTab(runDefWidget,"Analysis Parameters");
//	mainTabWindow_->addTab(outputDisplay_,"Analysis Output");
//	mainTabWindow_->setTabEnabled(2,false);
//
//	QVBoxLayout *mainLayout = new QVBoxLayout;
//	mainLayout->addLayout(toolbarLayout);
//	mainLayout->addWidget(progressBar_);
//	mainLayout->addWidget(mainTabWindow_);
//
//	setLayout(mainLayout);
//}
//
//void AnalysisViewer::loadSession()
//{
//	QStringList filenames = QFileDialog::getOpenFileNames(	this,
//															tr("Select sessions"),
//															".",
//															"Picto Sessions(*.sqlite)");
//	if(!filenames.size())
//		return;
//	filenames.sort();
//	foreach(QSqlDatabase session,sessions_)
//	{
//		if(session.isValid())
//			session.close();
//	}
//	sessions_.clear();
//	foreach(QString filename,filenames)
//	{
//		//Load Sqlite Database
//		filename = filename.replace("\\","/");
//		Q_ASSERT(filename.lastIndexOf("/") >=0);
//		Q_ASSERT(filename.lastIndexOf(".") >=0);
//		QString connectionName = filename.mid(filename.lastIndexOf("/")+1);
//		connectionName = connectionName.left(connectionName.lastIndexOf("."));
//
//		QSqlDatabase newSession = QSqlDatabase::addDatabase("QSQLITE",connectionName);
//		newSession.setDatabaseName(filename);
//		newSession.open();
//		if(!newSession.isOpen())
//		{
//			QMessageBox msg;
//			msg.setText("Error: Could not open session file.");
//			msg.exec();
//			return;
//		}
//		mainTabWindow_->setTabEnabled(2,false);
//		outputDisplay_->clear();
//		sessions_.append(newSession);
//		Q_ASSERT(sessions_.last().isValid() && sessions_.last().isOpen());
//	}
//	runSelector_->loadSessions(sessions_);
//	
//	updateUI();
//}
//
//void AnalysisViewer::saveOutput()
//{
//	//Restore dialog values
//	QString dirName = ".";
//	bool useSeperateSubDirs = true;
//	if(configDb_.isOpen())
//	{
//		QSqlQuery query(configDb_);
//		query.exec(QString("SELECT key,value FROM workstationinfo WHERE key IN ('OutputPath','SeperateDirs')"));
//		while(query.next())
//		{
//			if(query.value(0) == "OutputPath")
//				dirName = query.value(1).toString();
//			else if(query.value(0) == "SeperateDirs")
//				useSeperateSubDirs = query.value(1).toBool();
//		}
//	}
//	SaveOutputDialog saveDialog(this,dirName,useSeperateSubDirs);
//	saveDialog.showDialog();
//	savedOutputBoxState_ = saveDialog.getCurrentState();
//	dirName = saveDialog.getSelectedDir();
//	if(dirName.isEmpty())
//		return;
//	useSeperateSubDirs = saveDialog.useSeperateSubDirs();
//
//	//Save dialog values
//	if(configDb_.isOpen())
//	{
//		QSqlQuery query(configDb_);
//		query.prepare(QString("INSERT OR REPLACE INTO workstationinfo (key,value) VALUES ('OutputPath',:outputPath)"));
//		query.bindValue(":outputPath",dirName);
//		query.exec();
//		query.prepare(QString("INSERT OR REPLACE INTO workstationinfo (key,value) VALUES ('SeperateDirs',:seperateDirs)"));
//		query.bindValue(":seperateDirs",useSeperateSubDirs);
//		query.exec();
//	}
//	status_ = SAVING;
//	progressBar_->setRange(0,0);	//Starts progress bar busy indicator.
//	progressBarTimer_->start();
//	executeAction_->setEnabled(false);
//	loadSessionAction_->setEnabled(false);
//	saveOutputAction_->setEnabled(false);
//	outputDisplay_->saveOutputToDirectory(dirName,useSeperateSubDirs);
//	progressBarTimer_->stop();
//	progressBar_->setRange(0,100);	//Returns progress bar to normal range if it wasn't done in updateProgressBar.
//	progressBar_->setValue(100);
//	executeAction_->setEnabled(true);
//	loadSessionAction_->setEnabled(true);
//	saveOutputAction_->setEnabled(true);
//}
//
//void AnalysisViewer::executeCommand()
//{
//	QTime timer;
//	timer.start();
//	QSharedPointer<Design> anaDesign = designRoot_->getDesign("AnalysisContainer",0);
//	if(!anaDesign)
//	{
//		QMessageBox box;
//		box.setText("File does not contain an Analysis Container");
//		box.setIconPixmap(QPixmap(":/icons/x.png"));
//		box.exec();
//		return;
//	}
//	QString errors;
//	//Since we're getting rid of the old analysis system anyway, and some validation errors
//	//have creeped in for valid analyses, I'm just disabling this so that I can verify that 
//	//old analyses will still work during testing.
//	//if(!anaDesign->compiles(&errors))
//	//{
//	//	QMessageBox box;
//	//	box.setText("Syntax check failed                                       ");
//	//	box.setDetailedText(errors);
//	//	box.setIconPixmap(QPixmap(":/icons/x.png"));
//	//	box.exec();
//	//	return;
//	//}
//	analysisContainer_ = anaDesign->getRootAsset().staticCast<AnalysisContainer>();
//
//	//Start Analysis
//	status_ = ANALYZING;
//	this->setFocus();
//	executeAction_->setEnabled(false);
//	loadSessionAction_->setEnabled(false);
//	saveOutputAction_->setEnabled(false);
//	runSelector_->setEnabled(false);
//	anaDesigner_->setEnabled(false);
//	outputDisplay_->clear();
//	runsRemaining_ = 0;
//	progressBar_->setRange(0,0);	//Starts progress bar busy indicator.
//	progressBarTimer_->start();
//	runsRemaining_ = runSelector_->selectedRunCount();
//	for(int i=0;i<runSelector_->selectedRunCount();i++)
//	{
//		analysisContainer_->loadSession(runSelector_->getSessionForSelectedRun(i),true);
//		analysisContainer_->startNewRun(runSelector_->getSelectedRun(i));
//		QString result = analysisContainer_->run();
//		analysisContainer_->finish();
//		int currTabId = outputDisplay_->addTopLevelTab(runSelector_->nameOfSelectedRun(i));
//		QLinkedList<QPointer<AnalysisOutputWidget>> outputWidgets = analysisContainer_->getOutputWidgets();
//		foreach(QPointer<QWidget> widget, outputWidgets)
//		{
//			outputDisplay_->addSubTab(currTabId,widget->objectName(),widget);
//			mainTabWindow_->setTabEnabled(2,true);
//		}
//		runsRemaining_--;
//	}
//	progressBarTimer_->stop();
//	progressBar_->setRange(0,100);	//Returns progress bar to normal range if it wasn't done in updateProgressBar.
//	progressBar_->setValue(100);
//	QCoreApplication::processEvents();	//Get rid of any multiple presses on the execute button before we reenable it.
//	executeAction_->setEnabled(true);
//	loadSessionAction_->setEnabled(true);
//	runSelector_->setEnabled(true);
//	anaDesigner_->setEnabled(true);
//	if(outputDisplay_->supportsSaving())
//	{
//		saveOutputAction_->setEnabled(true);
//	}
//	qDebug("Total Analysis Time: " + QString::number(timer.elapsed()).toLatin1());
//}
//
//void AnalysisViewer::updateUI()
//{
//	if(sessions_.length() == 0)
//		currSessionLabel_->setText(tr("No Loaded Session"));
//	else if(sessions_.length() == 1)
//		currSessionLabel_->setText(sessions_[0].connectionName());
//	else
//		currSessionLabel_->setText(sessions_[0].connectionName()+" - " + sessions_.last().connectionName());
//	progressBar_->setRange(0,100);
//	progressBar_->reset();
//}
//
//void AnalysisViewer::updateProgressBar()
//{
//	progressBar_->setRange(0,100);
//	double totalFraction = 0;
//	switch(status_)
//	{
//	case ANALYZING:
//		{
//			if(!analysisContainer_)
//				return;
//			int currRunPercentRemaining = analysisContainer_->getPercentRemaining();
//			//The analysisContainer_ gives us the percent remaining of the current run,
//			//we need to update this value to reflect the portion of all runs completed.
//			double totalRuns = runSelector_->selectedRunCount();
//			double completed = totalRuns-runsRemaining_;
//			totalFraction = (completed+1.0-(double(currRunPercentRemaining)/100.0))/totalRuns;
//			
//		}
//	break;
//	case SAVING:
//		{
//			totalFraction = outputDisplay_->fractionOutputSaved();
//		}
//		break;
//	}
//	progressBar_->setValue(100.0*totalFraction);
//}