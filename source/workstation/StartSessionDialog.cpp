#include "StartSessionDialog.h"

#include "../common/protocol/ProtocolCommand.h"
#include "../common/protocol/ProtocolResponse.h"

#include <QToolButton>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamReader>
#include "../common/memleakdetect.h"


QString StartSessionDialog::filename_ = "";
QDir StartSessionDialog::currentDir_ = QDir(".");

StartSessionDialog::StartSessionDialog(QSharedPointer<Picto::CommandChannel> commandChannel)
:commandChannel_(commandChannel)
{
	createDialog();
}


void StartSessionDialog::createDialog()
{
	setWindowTitle(tr("Start Session"));
	//Create everything
	fileLabel_ = new QLabel(tr("Experiment"));
	fileEdit_ = new QLineEdit;
	fileEdit_->setText(filename_);
	moreFilesButton_ = new QToolButton();
	moreFilesButton_->setText("...");

	directorLabel_ = new QLabel(tr("Director"));
	directorComboBox_ = createDirectorList();

	buttonBox_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	//Lay everything out
	QVBoxLayout *mainLayout = new QVBoxLayout;
	QGridLayout *topLayout = new QGridLayout;

	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(buttonBox_);

	topLayout->setColumnMinimumWidth(1,10);
	topLayout->addWidget(directorLabel_,0,0);
	topLayout->addWidget(directorComboBox_,0,2);
	topLayout->addWidget(fileLabel_,1,0);
	topLayout->addWidget(fileEdit_,1,2);
	topLayout->addWidget(moreFilesButton_,1,3);

	setLayout(mainLayout);

	//connect everything
	connect(moreFilesButton_,SIGNAL(clicked()), this, SLOT(selectExperimentFile()));
	connect(buttonBox_, SIGNAL(accepted()), this, SLOT(loadExperiment()));
	connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject()));
}

//!	Creates a combo box and fills it with the currently active Director instances
QComboBox* StartSessionDialog::createDirectorList()
{
	QComboBox* box = new QComboBox;
	box->addItem(tr("LocalDirector"));
	box->setCurrentIndex(0);

	if(commandChannel_->getChannelStatus() != Picto::CommandChannel::connected)
	{
		QMessageBox msgBox;
		msgBox.setText(tr("No server found on network.\n\n All experiments will be run locally."));
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
		return box;
	}


	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand("DIRECTORLIST / PICTO/1.0"));
	QSharedPointer<Picto::ProtocolResponse> response;

	commandChannel_->sendCommand(command);
	if(commandChannel_->waitForResponse(1000))
	{
		response = commandChannel_->getResponse();
		
		QByteArray xmlFragment = response->getContent();
		QXmlStreamReader xmlReader(xmlFragment);

		while(!xmlReader.atEnd())
		{
			xmlReader.readNext();

			if(xmlReader.isStartElement() && xmlReader.name() == "Director")
			{
				ComponentInstance director;

				while(!xmlReader.atEnd())
				{
					xmlReader.readNext();

					if(xmlReader.name() == "Name" && xmlReader.isStartElement())
					{
						director.name = xmlReader.readElementText();
					}
					else if(xmlReader.name() == "Status" && xmlReader.isStartElement())
					{
						director.status = xmlReader.readElementText();
					}
					else if(xmlReader.name() == "Address" && xmlReader.isStartElement())
					{
						director.addr = xmlReader.readElementText();
					}
					else if(xmlReader.name() == "Id" && xmlReader.isStartElement())
					{
						director.id = xmlReader.readElementText();
					}
					else if(xmlReader.name() == "Director" && xmlReader.isEndElement())
					{
						directors_.append(director);
						break;
					}
				}
			}
		}

		for(int i=0; i<directors_.length(); i++)
		{
			box->insertItem(i+1,directors_[i].name);
		}
	}
	else
	{
		QMessageBox errMsg;
		errMsg.setText(tr("Server did not respond to request for list of Director instances."));
		errMsg.exec();
	}


	return box;
}

void StartSessionDialog::selectExperimentFile()
{
	QString fullFile = QFileDialog::getOpenFileName(this, tr("Select Experiment"),
								currentDir_.absolutePath(),
								tr("Experiments (*.exp *.xml);;All files (*.*)"));
	filename_ = fullFile.right(fullFile.length() - fullFile.lastIndexOf('/') - 1);
	currentDir_ = QDir(fullFile.left(fullFile.lastIndexOf('/')));

	fileEdit_->setText(filename_);
}

/*! \brief Returns the selected director's unique ID as a string.
 *
 *	The value is returned as a string (rather than a QUuid), because Workstation 
 *	never uses the ID as anything other than as an argument in a command
 *	sent to the server.  If no director is selected, an empty string is returned.
 */
QString StartSessionDialog::getDirectorID()
{
	if(directorComboBox_->currentIndex() == 0)
	{
		return QString();
	}
	else
	{
		return directors_[directorComboBox_->currentIndex()-1].id;
	}
}

void StartSessionDialog::loadExperiment()
{
	if(directorComboBox_->currentIndex() == 0)
	{
		// Implement a local director instance, or simulator...
	}
	else
	{
		//open the experiment xml file
		QFile xmlFile(currentDir_.absoluteFilePath(filename_));
		if(!xmlFile.open(QIODevice::ReadOnly))
		{
			QMessageBox msgBox;
			msgBox.setText(tr("Unable to open experiment file: \n  ") + filename_);
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.exec();
			return;
		}
		QByteArray xmlByteArr = xmlFile.readAll();

		QString commandStr;
		QString directorId = directors_[directorComboBox_->currentIndex()-1].id;
		QString directorAddr = directors_[directorComboBox_->currentIndex()-1].addr;
		commandStr = "STARTSESSION "+directorId+" PICTO/1.0";

		QSharedPointer<Picto::ProtocolCommand> loadExpCommand(new Picto::ProtocolCommand(commandStr));
		loadExpCommand->setContent(xmlByteArr);
		loadExpCommand->setFieldValue("Content-Length",QString("%1").arg(xmlByteArr.length()));

		//Before loading the experiment remotely, we should try it locally just to make sure that
		//the XML is legal.
		QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(xmlByteArr));
		while(!xmlReader->atEnd() && xmlReader->name().toString() != "Experiment")
			xmlReader->readNext();

		experiment_ = QSharedPointer<Picto::Experiment>(Picto::Experiment::Create());
		if(!experiment_->fromXml(xmlReader) || !experiment_->validateTree())
		{
			QMessageBox xmlError;
			xmlError.setText(tr("Error in Experiment XML"));
			xmlError.setDetailedText(experiment_->getErrors());
			xmlError.exec();
			return;
		}


		QSharedPointer<Picto::ProtocolResponse> loadExpResponse;

		if(commandChannel_->getChannelStatus() != Picto::CommandChannel::connected)
		{
			QMessageBox msgBox;
			msgBox.setText(tr("Not connected to server, unable to load experiment."));
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.exec();
			reject();
			return;
		}

		commandChannel_->sendCommand(loadExpCommand);
		if(!commandChannel_->waitForResponse(5000))
		{
			QMessageBox msgBox;
			msgBox.setText(tr("No response from server"));
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.exec();
			reject();
			return;
		}

		loadExpResponse = commandChannel_->getResponse();

		QMessageBox msgBox;

		if(loadExpResponse.isNull())
		{
			msgBox.setText(tr("No response received from server.\nExperiment not loaded."));
			msgBox.setIcon(QMessageBox::Critical);
		}
		else if(loadExpResponse->getResponseCode() == 404)
		{
			msgBox.setText(tr("Director instance not found with id:") + directorId +tr(" and IP address:") + directorAddr + tr("\nExperiment not loaded."));
			msgBox.setIcon(QMessageBox::Critical);
		}
		else if(loadExpResponse->getResponseCode() == 401)
		{
			msgBox.setText(tr("Director instance has already loaded an experiment.\nExperiment not loaded."));
			msgBox.setIcon(QMessageBox::Critical);
		}
		else if(loadExpResponse->getResponseCode() == 200)
		{

			QByteArray content = loadExpResponse->getDecodedContent();
			QXmlStreamReader xmlReader(content);

			while(!xmlReader.atEnd() && !(xmlReader.isStartElement() && xmlReader.name() == "SessionID"))
				xmlReader.readNext();

			if(!xmlReader.atEnd())
			{
				sessionId_ = QUuid(xmlReader.readElementText());
				msgBox.setText(tr("Experiment loaded on remote Director instance."));
				msgBox.setDetailedText(tr("Session ID: ") + sessionId_.toString());
			}
			else
			{
				msgBox.setText(tr("SessionID not found in response from server."));
			}
		}
		else
		{
			msgBox.setText(tr("Unexpected response from server.\nExperiment not loaded."));
			msgBox.setIcon(QMessageBox::Critical);
		}

		msgBox.exec();
		if(loadExpResponse->getResponseCode() == 200)
		{
			accept();
		}
		else
		{
			reject();
		}
		return;

	}

}