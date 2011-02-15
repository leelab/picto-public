#include <QtGui>

#include "proxymainwindow.h"
#include "NeuralDataAcqInterface.h"
#include "network/proxyserver.h"
#include "protocol/ProxyServerProtocols.h"
#include "protocol/ProxyServerAcqProtocol.h"
#include "../common/globals.h"
#include "../common/network/CommandChannel.h"
#include "../common/timing/timestamper.h"

ProxyMainWindow::ProxyMainWindow()
{
	acqPlugin_ = NULL;

    setWindowTitle(tr("%1").arg(Picto::Names->proxyServerAppName));

	createStatusLights();
	createButtons();
	createLineEdits();
	createComboBox();
	createLayout();
	createTimer();

	readSettings();

	QWidget *controlPanel = new QWidget(this);
	controlPanel->setLayout(layout_);
	
	setCentralWidget(controlPanel);
}

void ProxyMainWindow::setNeuralDataAcquisitionDevice(int index)
{
	//QAction *action = qobject_cast<QAction *>(sender());
	//acqPlugin_ = action->parent();
	acqPlugin_ = acqPluginList_[index];
	return;
}


















void ProxyMainWindow::startStopServer()
{
	if(startStopServerButton_->text() == startServerMsg)
		activate();
	else
		deActivate();
}

//This is called every 5 seconds just to confirm that the
//neural acquisition device hasn't failed out from under us.
void ProxyMainWindow::checkDevStatus()
{
	//Is the server supposed to be running?
	if(startStopServerButton_->text() == startServerMsg)
		return;

	//Is the device running?
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
	if(iNDAcq->getDeviceStatus() == NeuralDataAcqInterface::running)
		return;
	
	//attempt to recover
	iNDAcq->stopDevice();
	iNDAcq->startDevice();

	//did we recover?
	if(iNDAcq->getDeviceStatus() == NeuralDataAcqInterface::running)
		return;

	//stop device and server
	iNDAcq->stopDevice();
	startStopServer();

	//put up a message box
	QMessageBox deviceErrorBox;
	QString errorMsg = tr("Device \"%2\" has stopped running.  Confirm that "
						  "all the needed hardware and software is turned "
						  "on and started up, then restart the server.").
						  arg(iNDAcq->device());
	deviceErrorBox.setText(tr("Device not running"));
	deviceErrorBox.setInformativeText(errorMsg);
	deviceErrorBox.exec();
	return;
}

void ProxyMainWindow::closeEvent(QCloseEvent *ev)
{
	deActivate();

	writeSettings();
	//accept the close event
	ev->accept();
}

void ProxyMainWindow::serverActivity()
{
	activityTimer_->start();
	activityStatus_->turnGreen();
}

void ProxyMainWindow::createStatusLights()
{
	readyStatus_ = new StatusLight(this,Qt::red,10);
	activityStatus_ = new StatusLight(this,Qt::red,10);

	readyStatusLabel_ = new QLabel(tr("Ready"));
	activityStatusLabel_ = new QLabel(tr("Connected"));

}

void ProxyMainWindow::createComboBox()
{
	pluginCombo_ = new QComboBox();
	pluginCombo_->setEditable(false);

	//Run through the plugins adding them to the combo box as we go 
	//(both static and dynamic plugins)
	foreach (QObject *plugin, QPluginLoader::staticInstances())
	{
		NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(plugin);
		if(iNDAcq)
		{
			//QAction *action = new QAction(iNDAcq->device(), plugin);
			//connect(action, SIGNAL(triggered()),this, SLOT(setNeuralDataAcquisitionDevice()));
			//pluginCombo_->addAction(action);
			pluginCombo_->addItem(iNDAcq->device());
		}
	}
	
	QDir pluginsDir = QDir(qApp->applicationDirPath());

#if defined(Q_OS_WIN)
	if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
		pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
	if (pluginsDir.dirName() == "MacOS") {
		pluginsDir.cdUp();
		pluginsDir.cdUp();
		pluginsDir.cdUp();
	}
#endif
	pluginsDir.cd("plugins");

	foreach (QString fileName, pluginsDir.entryList(QDir::Files)) 
	{
		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
		QObject *plugin = loader.instance();
		//QString error = loader.errorString();
		if (plugin) 
		{
			NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(plugin);
			if(iNDAcq)
			{
				pluginCombo_->addItem(iNDAcq->device());
				acqPluginList_<<plugin;
			}
		}

	}

	if(acqPluginList_.length() == 0)
	{
		QMessageBox noPluginMsgBox;
		noPluginMsgBox.setText(tr("Plugins not found"));
		QString errorMsg = tr("No plugins for neural acquisition devices were found. "
							"Make sure there are %1 plugins in %2, then quit and restart "
							"%1").
							  arg(Picto::Names->proxyServerAppName).
							  arg(pluginsDir.absolutePath());
		noPluginMsgBox.setInformativeText(errorMsg);
		noPluginMsgBox.exec();

		startStopServerButton_->setEnabled(false);

		return;
	}

	setNeuralDataAcquisitionDevice(0);
	connect(pluginCombo_,SIGNAL(currentIndexChanged(int)),this,SLOT(setNeuralDataAcquisitionDevice(int)));

	return;
}

void ProxyMainWindow::createButtons()
{
	startServerMsg = tr("&Start");
	stopServerMsg_ = tr("&Stop");
	startStopServerButton_ = new QPushButton(startServerMsg);
	startStopServerButton_->setDefault(true);

	quitButton_ = new QPushButton(tr("&Quit"));
	quitButton_->setDefault(false);

	connect(startStopServerButton_,SIGNAL(clicked()),this,SLOT(startStopServer()));
	connect(quitButton_,SIGNAL(clicked()),this,SLOT(close()));
}

void ProxyMainWindow::createLineEdits()
{
	lineEditName_ = new QLineEdit();
	lineEditName_->setText("proxyName");
	lineEditNameLabel_ = new QLabel(tr("&Proxy Name:"));
	lineEditNameLabel_->setBuddy(lineEditName_);
}

void ProxyMainWindow::createLayout()
{
	QHBoxLayout *HLayout;
	layout_ = new QVBoxLayout();

	HLayout = new QHBoxLayout();
	HLayout->addWidget(lineEditNameLabel_);
	HLayout->addWidget(lineEditName_);
	layout_->addLayout(HLayout);

	HLayout = new QHBoxLayout();
	HLayout->addWidget(readyStatus_);
	HLayout->addWidget(readyStatusLabel_);
	layout_->addLayout(HLayout);
	
	HLayout = new QHBoxLayout();
	HLayout->addWidget(activityStatus_);
	HLayout->addWidget(activityStatusLabel_);
	layout_->addLayout(HLayout);

	layout_->addWidget(pluginCombo_);
	layout_->addWidget(startStopServerButton_);
	layout_->addWidget(quitButton_);
}

void ProxyMainWindow::createTimer()
{
	QTimer *statusTimer = new QTimer(this);
	connect(statusTimer,SIGNAL(timeout()),this,SLOT(checkDevStatus()));
	statusTimer->start(5000);

	activityTimer_ = new QTimer();
	activityTimer_->setInterval(2000);
	activityTimer_->setSingleShot(true);
	connect(activityTimer_,SIGNAL(timeout()),activityStatus_,SLOT(turnRed()));

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
void ProxyMainWindow::writeSettings()
{
	QSettings settings("Block Designs", Picto::Names->proxyServerAppName);

	settings.setValue("proxyName",lineEditName_->text());

}

void ProxyMainWindow::readSettings()
{
	QSettings settings("Block Designs", Picto::Names->proxyServerAppName);

	QString proxyName = settings.value("proxyName").toString();

	if(!proxyName.isNull())
		lineEditName_->setText(proxyName);
}


QString ProxyMainWindow::componentType()
{
	return "PROXY";
}
QString ProxyMainWindow::name()
{
	return lineEditName_->text().remove(' ');
}
int ProxyMainWindow::openDevice()
{
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
	//start the Neural Data Acquisition Device
	iNDAcq->startDevice();
	if(iNDAcq->getDeviceStatus() != NeuralDataAcqInterface::running)
	{
		QMessageBox startErrorBox;
		QString errorMsg = tr("%1 failed to start server.  "
							  "Device: \"%2\" is not running.  Confirm that "
							  "all the needed hardware and software is turned "
							  "on and started up, then try again.").
							  arg(Picto::Names->proxyServerAppName).
							  arg(iNDAcq->device());
		startErrorBox.setText(tr("Server failed to start"));
		startErrorBox.setInformativeText(errorMsg);
		startErrorBox.exec();
		return 1;
	}
	pluginCombo_->setEnabled(false);
	lineEditName_->setEnabled(false);
	startStopServerButton_->setText(stopServerMsg_);
	readyStatus_->turnGreen();
	return 0;
}
int ProxyMainWindow::closeDevice()
{
	startStopServerButton_->setText(startServerMsg);

	pluginCombo_->setEnabled(true);
	lineEditName_->setEnabled(true);
	readyStatus_->turnRed();
	return 0;
}
int ProxyMainWindow::startSession(QUuid sessionID)
{
	sessionEnded_ = false;
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
	forever
	{
		//send a PUTDATA command to the server with the most recent behavioral data
		QSharedPointer<Picto::ProtocolResponse> dataResponse;
		QString dataCommandStr = "PUTDATA "+name()+" PICTO/1.0";
		QSharedPointer<Picto::ProtocolCommand> response(new Picto::ProtocolCommand(dataCommandStr));

		//set up XML writer
		QString xmlData;
		QXmlStreamWriter writer(&xmlData);
		writer.setAutoFormatting(true);
		writer.writeStartDocument();
		writer.writeStartElement("Data");

		//Start writing the XML document
		writer.writeTextElement("device",iNDAcq->device());

		//check to see if our device is running
		writer.writeStartElement("deviceStatus");
		writer.writeCharacters("running");
		writer.writeEndElement();
		writer.writeTextElement("sampleRate",QString("%1").arg(iNDAcq->samplingRate()));


		//get the data from the neural acquisition device 
		//(the plugin should have formatted it as XML already)
		xmlData.append(iNDAcq->dumpData());
		writer.writeEndElement();  //end "ResponseACQ1.0"
		writer.writeEndDocument();
		
		response->setContent(xmlData.toUtf8());
		//response->setContentEncoding(Picto::ContentEncodingType::gzip); //ADD ENCODING!!!!!!!!!!!!!!!!
		response->setFieldValue("Content-Length",QString::number(xmlData.length()));

		dataCommandChannel_->sendRegisteredCommand(response);

		//check for and process responses
		while(dataCommandChannel_->waitForResponse(0))
		{
			serverActivity();
			dataResponse = dataCommandChannel_->getResponse();
			Q_ASSERT(!dataResponse.isNull());
			Q_ASSERT(dataResponse->getResponseType() == "OK");

			QString statusDirective = dataResponse->getDecodedContent().toUpper();

			//We may want to break this out in a seperate function at some point...
			if(statusDirective.startsWith("OK"))
			{
				//do nothing
			}
			else if(statusDirective.startsWith("STOP"))
			{
				//engine->stop();
			}
			else if(statusDirective.startsWith("PAUSE"))
			{
				//engine->pause();
			}
			else if(statusDirective.startsWith("RESUME"))
			{
				//engine->resume();
			}
			else if(statusDirective.startsWith("REWARD"))
			{
				//int channel = statusDirective.split(" ").value(1).toInt();
				//engine->giveReward(channel);	
			}
			else if(statusDirective.startsWith("ENDSESSION"))
			{
				serverUpdateChannel_->setSessionId(QUuid());
				sessionEnded_ = true;
				break;	
			}
			else
			{
				Q_ASSERT_X(false, "State::updateServer", "Unrecognized directive received from server: "+statusDirective.toAscii());
			}
		}
		if(sessionEnded_)
			break;
		//Pause for 20 ms 
		QEventLoop pauseLoop;
		QTimer pauseTimer;
		pauseTimer.setSingleShot(true);
		pauseTimer.setInterval(50);
		QObject::connect(&pauseTimer, SIGNAL(timeout()), &pauseLoop, SLOT(quit()));
		pauseTimer.start();
		pauseLoop.exec();
	}
	return 0;
}
int ProxyMainWindow::loadExp()
{
	return 0;
}
int ProxyMainWindow::startExp(QString expName)
{
	return 0;
}
int ProxyMainWindow::stopExp()
{
	return 0;
}
int ProxyMainWindow::reward(int channel)
{
	return 0;
}
int ProxyMainWindow::reportError()
{
	return 0;
}
int ProxyMainWindow::reportUnsupported()
{
	return 0;
}
int ProxyMainWindow::endSession()
{
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
	sessionEnded_ = true;
	iNDAcq->stopDevice();
	return 0;
}