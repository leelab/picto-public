#include <QtWidgets>

//#include "../common/common.h"
#include "../common/globals.h"
//#include "../common/namedefs.h"

#include "dialog.h"

Dialog::Dialog(SystemService * systemService, QWidget *parent) :	
  QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
	targetService(systemService)
{
	infoLabel = new QLabel(tr(
					 "You can use this dialog to install/remove and start/stop the\n"
					 "%1 Service/Daemon on this machine.\n\n"
					 "Installation allows %1 to automatically run in the\n"
					 "background at system startup, even if no one is logged in.\n\n"
					 "To manage %1 once installed and running, use %2.").arg(Picto::Names->proxyServerAppName)
																		.arg(Picto::Names->configAppName));
	infoLabel->setAlignment(Qt::AlignCenter);

	installButton = new QPushButton(tr("&Install %1 Service/Daemon").arg(Picto::Names->proxyServerAppName));
	removeButton = new QPushButton(tr("&Remove %1 Service/Daemon").arg(Picto::Names->proxyServerAppName));
	startButton = new QPushButton(tr("&Start %1 Service/Daemon").arg(Picto::Names->proxyServerAppName));
	stopButton = new QPushButton(tr("S&top %1 Service/Daemon").arg(Picto::Names->proxyServerAppName));

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
	}


	connect(installButton, SIGNAL(clicked()), this, SLOT(doInstall()));
	connect(removeButton, SIGNAL(clicked()), this, SLOT(doRemove()));
	connect(startButton, SIGNAL(clicked()), this, SLOT(doStart()));
	connect(stopButton, SIGNAL(clicked()), this, SLOT(doStop()));

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

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addStretch(1);
	mainLayout->addLayout(textLayout);
	mainLayout->addSpacing(15);
	mainLayout->addLayout(installRemoveButtonLayout);
	mainLayout->addLayout(startStopButtonLayout);
	mainLayout->addStretch(1);
	mainLayout->setSizeConstraint(QLayout::SetFixedSize);

	setLayout(mainLayout);

	setWindowTitle(Picto::Names->proxyServerAppName);
	setWindowFlags(Qt::Window);
}

void Dialog::doInstall()
{
	if(targetService->install())
	{
		installButton->setEnabled(false);
		removeButton->setEnabled(true);
		startButton->setEnabled(true);
		stopButton->setEnabled(false);
	}
	else
	{
		QMessageBox(QMessageBox::Critical,
			        tr("Service/Daemon Installation Error"),
					tr("Unable to install the %1 Service/Daemon.").arg(Picto::Names->proxyServerAppName),
					QMessageBox::Ok,
					this).exec();
	}
}

void Dialog::doRemove()
{
	if(targetService->remove())
	{
		installButton->setEnabled(true);
		removeButton->setEnabled(false);
		startButton->setEnabled(false);
		stopButton->setEnabled(false);
	}
	else
	{
		QMessageBox(QMessageBox::Critical,
			        tr("Service/Daemon Removal Error"),
					tr("Unable to remove the %1 Service/Daemon.").arg(Picto::Names->proxyServerAppName),
					QMessageBox::Ok,
					this).exec();
	}
}

void Dialog::doStart()
{
	if(targetService->start())
	{
		startButton->setEnabled(false);
		stopButton->setEnabled(true);
	}
	else
	{
		QMessageBox(QMessageBox::Critical,
			        tr("Service/Daemon Start Error"),
					tr("Unable to start the %1 Service/Daemon.").arg(Picto::Names->proxyServerAppName),
					QMessageBox::Ok,
					this).exec();
	}
}

void Dialog::doStop()
{
	if(targetService->stop())
	{
		startButton->setEnabled(true);
		stopButton->setEnabled(false);
	}
	else
	{
		QMessageBox(QMessageBox::Critical,
			        tr("Service/Daemon Stop Error"),
					tr("Unable to stop the %1 Service/Daemon.").arg(Picto::Names->proxyServerAppName),
					QMessageBox::Ok,
					this).exec();
	}
}
