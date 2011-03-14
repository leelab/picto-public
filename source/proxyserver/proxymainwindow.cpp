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

//! \brief Sets the plugin to the value in the plugin list
void ProxyMainWindow::setNeuralDataAcquisitionDevice(int index)
{
	acqPlugin_ = acqPluginList_[index];
	return;
}

//! \brief Called by the button signal, this starts or stops ProxyClient activity.
void ProxyMainWindow::startStopClient()
{
	if(startStopClientButton_->text() == startServerMsg)
		activate();
	else
		deActivate();
}

//! \brief This is called every 5 seconds just to confirm that the neural acquisition device hasn't failed out from under us.
void ProxyMainWindow::checkDevStatus()
{
	//Is the server supposed to be running?
	if(startStopClientButton_->text() == startServerMsg)
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
	startStopClient();

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

//! \brief Called when the window is closed, this ends all activity and saves settings
void ProxyMainWindow::closeEvent(QCloseEvent *ev)
{
	deActivate();

	writeSettings();
	//accept the close event
	ev->accept();
}

//! \brief This signals to the UI that activity is detected from the server.
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

		startStopClientButton_->setEnabled(false);

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
	startStopClientButton_ = new QPushButton(startServerMsg);
	startStopClientButton_->setDefault(true);

	quitButton_ = new QPushButton(tr("&Quit"));
	quitButton_->setDefault(false);

	connect(startStopClientButton_,SIGNAL(clicked()),this,SLOT(startStopClient()));
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
	layout_->addWidget(startStopClientButton_);
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
	startStopClientButton_->setText(stopServerMsg_);
	readyStatus_->turnGreen();
	return 0;
}
int ProxyMainWindow::closeDevice()
{
	startStopClientButton_->setText(startServerMsg);

	pluginCombo_->setEnabled(true);
	lineEditName_->setEnabled(true);
	readyStatus_->turnRed();
	return 0;
}

/*! \brief When a NEWSESSION is started, proxy starts sending data to the server.
 *	This runs until endSession is called or the Session ends.
 */
int ProxyMainWindow::startSession(QUuid sessionID)
{
	sessionEnded_ = false;
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
	forever
	{
		//send a PUTDATA command to the server with the most recent behavioral data
		QSharedPointer<Picto::ProtocolResponse> dataResponse;
		QString dataCommandStr = "PUTDATA "+name()+":"+ getStatusString() + " PICTO/1.0";
		QSharedPointer<Picto::ProtocolCommand> response(new Picto::ProtocolCommand(dataCommandStr));

		//set up XML writer
		QString xmlData;
		QSharedPointer<QXmlStreamWriter> writer(new QXmlStreamWriter(&xmlData));
		writer->setAutoFormatting(true);
		writer->writeStartDocument();
		writer->writeStartElement("Data");

		//Start writing the XML document
		//WE ARE NOT CURRENTLY USING THE DATA BELOW, SO ITS DISABLED, BUT WE WILL
		//NEED TO.  WHEN THIS HAPPENS, PUT IT IN A DATASTORE OF ITS OWN AND HANDLE
		//IT IN PUTDATA HANDLER ON SERVER.
		//writer->writeTextElement("device",iNDAcq->device());

		////check to see if our device is running
		//writer->writeStartElement("deviceStatus");
		//writer->writeCharacters("running");
		//writer->writeEndElement();
		//writer->writeTextElement("sampleRate",QString("%1").arg(iNDAcq->samplingRate()));


		//get the data from the neural acquisition device 
		//(the plugin should have formatted it as XML already)
		QList<QSharedPointer<Picto::DataStore>> dataList = iNDAcq->dumpData();
		foreach(QSharedPointer<Picto::DataStore> data, dataList)
		{
			data->serializeAsXml(writer);
		}
		//xmlData.append(iNDAcq->dumpData());
		writer->writeEndElement();  //end "ResponseACQ1.0"
		writer->writeEndDocument();
		
		response->setContent(xmlData.toUtf8());
		//response->setContentEncoding(Picto::ContentEncodingType::gzip); //ADD ENCODING!!!!!!!!!!!!!!!!
		response->setFieldValue("Content-Length",QString::number(xmlData.length()));
		
		if(dataList.size())
		{
			qDebug("sent message");
			dataCommandChannel_->sendRegisteredCommand(response);
		}

		//check for and process responses
		if(dataCommandChannel_->assureConnection(2000))
		{
			serverActivity();
			while(dataCommandChannel_->waitForResponse(0))
			{
qDebug("responses waiting");
				dataResponse = dataCommandChannel_->getResponse();
				Q_ASSERT(!dataResponse.isNull());
				Q_ASSERT(dataResponse->getResponseType() == "OK");

				QString statusDirective = dataResponse->getDecodedContent().toUpper();
qDebug("got message");
				//We may want to break this out in a seperate function at some point...
				if(statusDirective.startsWith("OK"))
				{
					qDebug("Message:OK");
					//do nothing
				}
				else if(statusDirective.startsWith("STOP"))
				{
					qDebug("Message:STOP");
					//engine->stop();
				}
				else if(statusDirective.startsWith("PAUSE"))
				{
					qDebug("Message:PAUSE");
					//engine->pause();
				}
				else if(statusDirective.startsWith("RESUME"))
				{
					qDebug("Message:RESUME");
					//engine->resume();
				}
				else if(statusDirective.startsWith("REWARD"))
				{
					qDebug("Message:REWARD");
					//int channel = statusDirective.split(" ").value(1).toInt();
					//engine->giveReward(channel);	
				}
				else if(statusDirective.startsWith("ENDSESSION"))
				{
					qDebug("Message:ENDSESSION");
					sessionEnded_ = true;
					break;	
				}
				else
				{
					qDebug("Message:UNKNOWN DIRECTIVE");
					Q_ASSERT_X(false, "State::updateServer", "Unrecognized directive received from server: "+statusDirective.toAscii());
				}
			}

			//resend all of the pending responses
			dataCommandChannel_->resendPendingCommands(10);
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
	setStatus(idle);
	return 0;
}
int ProxyMainWindow::endSession()
{
	NeuralDataAcqInterface *iNDAcq = qobject_cast<NeuralDataAcqInterface *>(acqPlugin_);
	sessionEnded_ = true;
	iNDAcq->stopDevice();
	return 0;
}