#ifndef _SAVEOUTPUT_DIALOG_H_
#define _SAVEOUTPUT_DIALOG_H_

#include <QFileDialog>

class QLabel;
class QCheckBox;

/*!	\brief A dialog for saving analysis output.
 *	\details Qt provides some really easy ways to do a save file dialog, but we couldn't
 *	use them for saving output because we wanted to add an option for saving Output files
 *	into separate directories by run name instead of putting them all into the same directory.
 *	For that reason, we made this custom SaveOutputDialog that includes that extra checkbox.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class SaveOutputDialog : public QFileDialog
{
	Q_OBJECT
public:
	SaveOutputDialog(QWidget *parent, QString savePath = ".", bool defaultSubDirSetting = true);
	virtual ~SaveOutputDialog(){};

	void showDialog();
	QString getSelectedDir();
	bool useSeperateSubDirs();
	QByteArray getCurrentState();

private:
	void setup();
	void addCheckBox();

	QLabel *subDirOpLabel_;
	QCheckBox *subDirOpCheck_;

	QString defaultPath_;
	QString selectedDir_;
	bool defaultSubDirSetting_;
};

#endif