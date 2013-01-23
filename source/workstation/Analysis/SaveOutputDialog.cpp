#include <QLabel>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QBoxLayout>
#include <QDir>

#include "SaveOutputDialog.h"
#include "../common/memleakdetect.h"

SaveOutputDialog::SaveOutputDialog(QWidget *parent, QString savePath, bool defaultSubDirSetting)
: QFileDialog(parent,tr("Select Output Directory"),"."),
defaultPath_(savePath),
defaultSubDirSetting_(defaultSubDirSetting)
{
	setup();
	addCheckBox();
}

void SaveOutputDialog::showDialog()
{
	exec();
}

QString SaveOutputDialog::getSelectedDir()
{
	QStringList selectedDirs = selectedFiles();
	if(selectedDirs.length())
		return selectedDirs.first();
	return "";
}

bool SaveOutputDialog::useSeperateSubDirs()
{
	return subDirOpCheck_->isChecked();
}

QByteArray SaveOutputDialog::getCurrentState()
{
	return saveState();
}

void SaveOutputDialog::setup()
{
	if(QDir(defaultPath_).exists())
		setDirectory(defaultPath_);
	setOption(QFileDialog::ShowDirsOnly,true);
	setFileMode(QFileDialog::Directory);
}

//! Adds "use seperate subdirs" checkbox to the dialog
void SaveOutputDialog::addCheckBox()
{
	QDialogButtonBox *box = findChild<QDialogButtonBox*>();
	Q_ASSERT(box);
	QBoxLayout *l = box->findChild<QBoxLayout*>();
	Q_ASSERT(l);
	subDirOpCheck_ = new QCheckBox("Save each run in a separate sub-directory.", box);
	subDirOpCheck_->setChecked(defaultSubDirSetting_);
	l->insertWidget(0, subDirOpCheck_);
}