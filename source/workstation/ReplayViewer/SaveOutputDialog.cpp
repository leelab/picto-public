#include <QLabel>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QBoxLayout>
#include <QDir>

#include "SaveOutputDialog.h"
#include "../common/memleakdetect.h"

/*! \brief Constructs a SaveOutputDialog with the input default savePath and the input default
 *	setting for saving each run in a separate sub directory.
 */
SaveOutputDialog::SaveOutputDialog(QWidget *parent, QString savePath, bool defaultSubDirSetting)
: QFileDialog(parent,tr("Select Output Directory"),"."),
defaultPath_(savePath),
defaultSubDirSetting_(defaultSubDirSetting)
{
	setup();
	addCheckBox();
}

/*! \brief Presents the the dialog UI to the user and gets the results which can be accessed
 *	with getSelectedDir() and useSeparatedSubDirs().
 */
void SaveOutputDialog::showDialog()
{
	selectedDir_.clear();
	if(exec())
	{
		QStringList selectedDirs = selectedFiles();
		if(selectedDirs.length())
			selectedDir_ = selectedDirs.first();
	}
}

/*! \brief Returns the directory that as selected in the dialog.*/
QString SaveOutputDialog::getSelectedDir()
{
	return selectedDir_;
}

/*! \brief Returns the directory that was selected in the dialog.*/
bool SaveOutputDialog::useSeperateSubDirs()
{
	return subDirOpCheck_->isChecked();
}

/*! \brief This returns QFileDialog::saveState() which is used to save the state of the dialog.
 *	It could be used with QFileDialog::restoreState(), but we aren't doing that right now.
 */
QByteArray SaveOutputDialog::getCurrentState()
{
	return saveState();
}

/*! \brief Sets up the SaveOutputDialog parameters.
*/
void SaveOutputDialog::setup()
{
	if(QDir(defaultPath_).exists())
		setDirectory(defaultPath_);
	setOption(QFileDialog::ShowDirsOnly,true);
	setFileMode(QFileDialog::Directory);
	selectedDir_ = "";
}

/*! \brief Adds a "use seperate subdirs" checkbox to the dialog.
 */
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