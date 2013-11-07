#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileInfo>
#include <QButtonGroup>
#include <QPushButton>
#include <QToolButton>
#include "AutoSaveDialog.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
AutoSaveDialog::AutoSaveDialog(QStringList files,QWidget *parent) :
	QDialog(parent,Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint),
	deleteIcon_(":/icons/delete.png")
{
	files_ = files;
	setAttribute(Qt::WA_DeleteOnClose);
	
	mainWidget_ = new QWidget();
	scrollArea_ = new QScrollArea();
	scrollArea_->setBackgroundRole(QPalette::Light);
	scrollArea_->setWidget(mainWidget_);
	restoreButton_ = new QPushButton("Open");
	connect(restoreButton_,SIGNAL(clicked(bool)),this,SLOT(restoreTriggered(bool)));
	ignoreButton_ = new QPushButton("Ignore");
	connect(ignoreButton_,SIGNAL(clicked(bool)),this,SLOT(ignoreTriggered(bool)));

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(restoreButton_);
	buttonLayout->addWidget(ignoreButton_);
	QVBoxLayout* layout(new QVBoxLayout());
	instructions_ = new QLabel("Label");
	layout->addWidget(instructions_,0);
	layout->addWidget(scrollArea_,1);
	layout->addLayout(buttonLayout,0);
	setLayout(layout);
	setWindowTitle("Restore Design");
	updateContents();
}

void AutoSaveDialog::updateContents()
{
	Q_ASSERT(files_.size());
	
	buttonGroup_ = QSharedPointer<QButtonGroup>(new QButtonGroup());
	deleteButtonGroup_ = QSharedPointer<QButtonGroup>(new QButtonGroup());
	filePathLookup_.clear();
	connect(deleteButtonGroup_.data(),SIGNAL(buttonClicked(int)),this,SLOT(deleteTriggered(int)));
	
	//Remove all old buttons from layout.  We do this in a roundabout but simple way.  Moving a widget's
	//layout to another widget and then destroying that widget destroys the layout and all the children that
	//were attached to it.
	if(mainWidget_->layout())
		QWidget().setLayout(mainWidget_->layout());
	QVBoxLayout* layout(new QVBoxLayout());
	layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
	mainWidget_->setLayout(layout);
	
	if(files_.size() > 1)
		instructions_->setText("The following designs were automatically saved before Picto ended unexpectedly.\nPlease select and open a design, delete unnecessary designs, or ignore and continue.");
	else
		instructions_->setText("The following design was automatically saved before Picto ended unexpectedly.\nPlease open the design, delete it, or ignore and continue.");

	int buttonIndex = 0;
	foreach(QString file,files_)
	{
		QHBoxLayout* fileLayout = new QHBoxLayout();
		QCheckBox* checkBox(new QCheckBox(QFileInfo(file).fileName()));
		connect(checkBox,SIGNAL(clicked(bool)),this,SLOT(checkboxChanged(bool)));
		buttonGroup_->addButton(checkBox,buttonIndex);
		fileLayout->addWidget(checkBox,Qt::AlignLeft);
		QToolButton* deleteButton(new QToolButton());
		deleteButton->setIcon(deleteIcon_);
		deleteButtonGroup_->addButton(deleteButton,buttonIndex);
		filePathLookup_[buttonIndex] = file;
		fileLayout->addWidget(deleteButton,Qt::AlignRight);
		layout->addLayout(fileLayout);
		layout->setStretch(buttonIndex,0);
		buttonIndex++;
	}
	buttonGroup_->button(0)->setChecked(true);
	//layout->addStretch(1);
}

void AutoSaveDialog::checkboxChanged(bool)
{
}

void AutoSaveDialog::deleteTriggered(int buttonIndex)
{
	Q_ASSERT(filePathLookup_.contains(buttonIndex));
	emit deleteFileRequest(filePathLookup_.value(buttonIndex));
	files_.removeAt(buttonIndex);
	if(files_.size())
	{
		//Call updateContents() using the event loop.  Don't do it here, because we don't want
		//to end up deleting the object that triggered the call to this function while we're
		//still in this function.
		QTimer::singleShot(0, this, SLOT(updateContents()));
	}
	else
		done(0);
}

void AutoSaveDialog::restoreTriggered(bool)
{
	done(0);
	int selectedIndex = buttonGroup_->checkedId();
	if(selectedIndex >= 0)
	{
		Q_ASSERT(filePathLookup_.contains(selectedIndex));
		emit restoreFileRequest(filePathLookup_[selectedIndex]);
	}
}

void AutoSaveDialog::ignoreTriggered(bool)
{
	done(0);
}
