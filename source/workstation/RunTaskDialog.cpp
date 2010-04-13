#include "RunTaskDialog.h"

#include "../common/protocol/ProtocolCommand.h"
#include "../common/protocol/ProtocolResponse.h"

#include <QDialogButtonBox>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

RunTaskDialog::RunTaskDialog(QSharedPointer<Picto::CommandChannel> commandChannel, QSharedPointer<Picto::Experiment> experiment, QUuid sessionId)
	: commandChannel_(commandChannel),
	  experiment_(experiment),
	  sessionId_(sessionId)
{
	createDialog();
}

//! Creates the dialog
void RunTaskDialog::createDialog()
{
	setWindowTitle(tr("Run Task"));

	taskLabel_ = new QLabel(tr("Task"));

	taskComboBox_ = new QComboBox();
	QStringList taskList = experiment_->getTaskNames();
	taskList.sort();
	taskComboBox_->addItems(taskList);

	buttonBox_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	//Lay everything out
	QVBoxLayout *mainLayout = new QVBoxLayout;
	QHBoxLayout *topLayout = new QHBoxLayout;

	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(buttonBox_);

	topLayout->addWidget(taskLabel_);
	topLayout->addWidget(taskComboBox_);

	setLayout(mainLayout);

	//connect everything
	connect(buttonBox_, SIGNAL(accepted()), this, SLOT(runTask()));
	connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject()));
}

//! Runs the selected task
void RunTaskDialog::runTask()
{
	QString commandStr;
	QString modifiedTaskName = taskComboBox_->currentText();
	modifiedTaskName = modifiedTaskName.simplified();
	modifiedTaskName.remove(' ');
	commandStr = "STARTTASK "+modifiedTaskName+" PICTO/1.0";

	QSharedPointer<Picto::ProtocolCommand> startTaskCmd(new Picto::ProtocolCommand(commandStr));
	QSharedPointer<Picto::ProtocolResponse> startTaskResponse;
	startTaskCmd->setFieldValue("Session-ID",sessionId_.toString());

	if(commandChannel_->getChannelStatus() != Picto::CommandChannel::connected)
	{
		QMessageBox msgBox;
		msgBox.setText(tr("Not connected to server, unable to run task."));
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();
		reject();
		return;
	}

	commandChannel_->sendCommand(startTaskCmd);
	if(!commandChannel_->waitForResponse(5000))
	{
		QMessageBox msgBox;
		msgBox.setText(tr("No response from server"));
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();
		reject();
		return;
	}

	startTaskResponse = commandChannel_->getResponse();

	QMessageBox msgBox;

	if(startTaskResponse->getResponseCode() == 200)
	{
		accept();
		return;
	}
	else if(startTaskResponse.isNull())
	{
		msgBox.setText(tr("No response received from server.\nTask not started."));
		msgBox.setIcon(QMessageBox::Critical);
	}
	else if(startTaskResponse->getResponseCode() == 400)
	{
		msgBox.setText(tr("Error starting task: ") + startTaskResponse->getDecodedContent());
		msgBox.setIcon(QMessageBox::Critical);
	}
	else
	{
		msgBox.setText(tr("Unexpected response from server.\nExperiment not loaded."));
		msgBox.setIcon(QMessageBox::Critical);
	}

	msgBox.exec();
	reject();
}