#include <QtWidgets>

//#include "../common/common.h"
#include "../common/globals.h"
//#include "../common/namedefs.h"

#include "dialog.h"
#include "../common/memleakdetect.h"

/*! \brief Constructs a Dialog object to handle starting/stopping/installing/uninstalling the input SystemService.
 */
Dialog::Dialog(SystemService * systemService, QWidget *parent) :	
  QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	targetService(systemService)
{
	infoLabel = new QLabel(tr(
					 "You can use this dialog to install/remove and start/stop the\n"
					 "%1 Service/Daemon on this machine.\n\n"
					 "Installation allows %1 to automatically run in the\n"
					 "background at system startup, even if no one is logged in.\n\n"
					 "To manage %1 once installed and running, use %2.").arg(Picto::Names->serverAppName)
																		.arg(Picto::Names->configAppName));
	infoLabel->setAlignment(Qt::AlignCenter);

	installButton = new QPushButton(tr("&Install %1 Service/Daemon").arg(Picto::Names->serverAppName));
	removeButton = new QPushButton(tr("&Remove %1 Service/Daemon").arg(Picto::Names->serverAppName));
	startButton = new QPushButton(tr("&Start %1 Service/Daemon").arg(Picto::Names->serverAppName));
	stopButton = new QPushButton(tr("S&top %1 Service/Daemon").arg(Picto::Names->serverAppName));
	systemNumberBox = new QSpinBox();
	systemNumberBox->setMaximum(130);
	systemNumberBox->setValue(Picto::portNums->getSystemNumber());

	if(systemService->isInstalled())
	{
		installButton->setEnabled(false);

		if(systemService->isRunning())
		{
			startButton->setEnabled(false);
		}
		else
		{
			stopButton->setEnabled(false);
		}
	}
	else
	{
		removeButton->setEnabled(false);
		startButton->setEnabled(false);
		stopButton->setEnabled(false);
		systemNumberBox->setEnabled(true);
	}


	connect(installButton, SIGNAL(clicked()), this, SLOT(doInstall()));
	connect(removeButton, SIGNAL(clicked()), this, SLOT(doRemove()));
	connect(startButton, SIGNAL(clicked()), this, SLOT(doStart()));
	connect(stopButton, SIGNAL(clicked()), this, SLOT(doStop()));
	connect(systemNumberBox, SIGNAL(valueChanged(int)),this,SLOT(systemNumberChanged(int)));

	QHBoxLayout *textLayout = new QHBoxLayout;
	textLayout->addStretch(1);
	textLayout->addWidget(infoLabel);
	textLayout->addStretch(1);

	QHBoxLayout *installRemoveButtonLayout = new QHBoxLayout;
	installRemoveButtonLayout->addStretch(1);
	installRemoveButtonLayout->addWidget(installButton);
	installRemoveButtonLayout->addWidget(removeButton);
	installRemoveButtonLayout->addStretch(1);

	QHBoxLayout *startStopButtonLayout = new QHBoxLayout;
	startStopButtonLayout->addStretch(1);
	startStopButtonLayout->addWidget(startButton);
	startStopButtonLayout->addWidget(stopButton);
	startStopButtonLayout->addStretch(1);
	
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch(1);
	buttonLayout->addWidget(installButton);
	buttonLayout->addWidget(removeButton);
	buttonLayout->addStretch(1);
	QHBoxLayout *systemNumberLayout = new QHBoxLayout;
	systemNumberLayout->addWidget(new QLabel("System Number"));
	systemNumberLayout->addWidget(systemNumberBox);
	systemNumberLayout->addStretch(1);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addStretch(1);
	mainLayout->addLayout(textLayout);
	mainLayout->addSpacing(15);
	mainLayout->addLayout(installRemoveButtonLayout);
	mainLayout->addLayout(startStopButtonLayout);
	mainLayout->addLayout(systemNumberLayout);
	mainLayout->addStretch(1);
	mainLayout->setSizeConstraint(QLayout::SetFixedSize);

	setLayout(mainLayout);

	setWindowTitle(Picto::Names->serverAppName);
	setWindowFlags(Qt::Window);
}

/*! \brief Installs the SystemService to the OS using SystemService::install().  Shows an error message box if installation fails.
*/
void Dialog::doInstall()
{
	if(targetService->install())
	{
		installButton->setEnabled(false);
		removeButton->setEnabled(true);
		startButton->setEnabled(true);
		stopButton->setEnabled(false);
		systemNumberBox->setEnabled(false);
	}
	else
	{
		QMessageBox(QMessageBox::Critical,
			        tr("Service/Daemon Installation Error"),
					tr("Unable to install the %1 Service/Daemon.").arg(Picto::Names->serverAppName),
					QMessageBox::Ok,
					this).exec();
	}
}

/*! \brief Removes the SystemService from the OS using SystemService::remove().  Shows an error message box if removal fails.
*/
void Dialog::doRemove()
{
	if(targetService->remove())
	{
		installButton->setEnabled(true);
		removeButton->setEnabled(false);
		startButton->setEnabled(false);
		stopButton->setEnabled(false);
		systemNumberBox->setEnabled(true);
	}
	else
	{
		QMessageBox(QMessageBox::Critical,
			        tr("Service/Daemon Removal Error"),
					tr("Unable to remove the %1 Service/Daemon.").arg(Picto::Names->serverAppName),
					QMessageBox::Ok,
					this).exec();
	}
}

/*! \brief Starts the SystemService using SystemService::start().  Shows an error message box if the service cannot start.
*/
void Dialog::doStart()
{
	if(targetService->start())
	{
		startButton->setEnabled(false);
		stopButton->setEnabled(true);
		systemNumberBox->setEnabled(false);
	}
	else
	{
		QMessageBox(QMessageBox::Critical,
			        tr("Service/Daemon Start Error"),
					tr("Unable to start the %1 Service/Daemon.").arg(Picto::Names->serverAppName),
					QMessageBox::Ok,
					this).exec();
	}
}

/*! \brief Stops the SystemService using SystemService::stop().  Shows an error message box if the service cannot stop.
*/
void Dialog::doStop()
{
	if(targetService->stop())
	{
		startButton->setEnabled(true);
		stopButton->setEnabled(false);
		systemNumberBox->setEnabled(false);
	}
	else
	{
		QMessageBox(QMessageBox::Critical,
			        tr("Service/Daemon Stop Error"),
					tr("Unable to stop the %1 Service/Daemon.").arg(Picto::Names->serverAppName),
					QMessageBox::Ok,
					this).exec();
	}
}

/*! \brief Called when the user changes the system number using the spinbox.  Updates the system number using Picto::portNums::setSystemNumber().
*/
void Dialog::systemNumberChanged(int num)
{
	int currSysNum = Picto::portNums->getSystemNumber();
	if(num == currSysNum)
		return;
	Picto::portNums->setSystemNumber(QCoreApplication::applicationFilePath(),QCoreApplication::arguments(),num,false);
}
