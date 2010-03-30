#include "LoadExperimentDialog.h"

#include "../common/network/ServerDiscoverer.h"
#include "../common/protocol/ProtocolCommand.h"
#include "../common/protocol/ProtocolResponse.h"

#include <QToolButton>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>
#include <QXmlStreamReader>


QString LoadExperimentDialog::filename = "";
QDir LoadExperimentDialog::currentDir = QDir(".");

LoadExperimentDialog::LoadExperimentDialog()
{
	createDialog();
}

void LoadExperimentDialog::findServer()
{
	Picto::ServerDiscoverer discoverer;

	commandChannel = new Picto::CommandChannel();
	commandChannel->pollingMode(true);

	connect(&discoverer, SIGNAL(foundServer(QHostAddress, quint16)), commandChannel, SLOT(connectToServer(QHostAddress, quint16)));

	int maxTime = 30000;
	QProgressDialog prog("Looking for Server","Cancel",0,maxTime,this);
	prog.setWindowModality(Qt::WindowModal);


	for(int i=0; i<maxTime; i += 250)
	{
		prog.setValue(i);
		if(prog.wasCanceled())
			break;

		discoverer.discover(250);
		if(discoverer.waitForDiscovered(250))
			break;
	}
	prog.setValue(maxTime);

	if(commandChannel->getChannelStatus() != Picto::CommandChannel::connected)
	{
		QMessageBox msgBox;
		msgBox.setText("No server found on network.\n\n All experiments will be run locally.");
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}

}
void LoadExperimentDialog::createDialog()
{
	setWindowTitle(tr("Load Experiment"));
	//Create everything
	fileLabel = new QLabel(tr("Experiment"));
	fileEdit = new QLineEdit;
	fileEdit->setText(filename);
	moreFilesButton = new QToolButton();
	moreFilesButton->setText("...");

	directorLabel = new QLabel("Director");
	directorList = createDirectorList();

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	//Lay everything out
	QVBoxLayout *mainLayout = new QVBoxLayout;
	QGridLayout *topLayout = new QGridLayout;

	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(buttonBox);

	topLayout->setColumnMinimumWidth(1,10);
	topLayout->addWidget(directorLabel,0,0);
	topLayout->addWidget(directorList,0,2);
	topLayout->addWidget(fileLabel,1,0);
	topLayout->addWidget(fileEdit,1,2);
	topLayout->addWidget(moreFilesButton,1,3);

	setLayout(mainLayout);

	//connect everything
	connect(moreFilesButton,SIGNAL(clicked()), this, SLOT(selectExperimentFile()));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

//!	Creates a combo box and fills it with the currently active Director instances
QComboBox* LoadExperimentDialog::createDirectorList()
{
	QComboBox* box = new QComboBox;
	box->addItem("Local Director");

	findServer();

	if(commandChannel->getChannelStatus() == Picto::CommandChannel::connected)
	{
		QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand("DIRECTORLIST / PICTO/1.0"));
		QSharedPointer<Picto::ProtocolResponse> response;

		commandChannel->sendCommand(command);
		if(commandChannel->waitForResponse(1000))
		{
			response = commandChannel->getResponse();
			
			QByteArray xmlFragment = response->getContent();
			QXmlStreamReader xmlReader(xmlFragment);

			while(!xmlReader.atEnd())
			{
				xmlReader.readNext();

				if(xmlReader.isStartElement() && xmlReader.name() == "Name")
				{
					QString directorName = xmlReader.readElementText();
					box->addItem(directorName);
				}
			}
		}
	}

	box->setCurrentIndex(0);
	return box;
}

void LoadExperimentDialog::selectExperimentFile()
{
	QString fullFile = QFileDialog::getOpenFileName(this, tr("Select Experiment"),
								currentDir.absolutePath(),
								tr("Experiments (*.exp *.xml);;All files (*.*)"));
	filename = fullFile.right(fullFile.length() - fullFile.lastIndexOf('/') - 1);
	currentDir = QDir(fullFile.left(fullFile.lastIndexOf('/')));

	fileEdit->setText(filename);
}
