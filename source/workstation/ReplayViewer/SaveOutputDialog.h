#ifndef _SAVEOUTPUT_DIALOG_H_
#define _SAVEOUTPUT_DIALOG_H_

#include <QFileDialog>

class QLabel;
class QCheckBox;

/*!	\brief A dialog for saving analysis output.
 *
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
	bool defaultSubDirSetting_;
};

#endif