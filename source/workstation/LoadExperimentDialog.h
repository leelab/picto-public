#ifndef _LOAD_EXPERMINET_DIALOG_H_
#define _LOAD_EXPERMINET_DIALOG_H_

#include <QDialog>
#include <QDir>
#include <QMap>
#include <QHostAddress>

#include "../common/network/CommandChannel.h"

class QToolButton;
class QComboBox;
class QLabel;
class QLineEdit;
class QDialogButtonBox;


class LoadExperimentDialog : public QDialog
{
	Q_OBJECT
public:
	LoadExperimentDialog();

	QString getExperimentFile() { return currentDir.absoluteFilePath(filename); };
	QHostAddress getDirectorAddress() { return QHostAddress(); };

private slots:
	void selectExperimentFile();

private:
	void createDialog();
	void findServer();
	QComboBox* createDirectorList();

	QLabel *fileLabel;
	QLabel *directorLabel;
	QLineEdit *fileEdit;
	QToolButton *moreFilesButton;
	QComboBox *directorList;
	QDialogButtonBox *buttonBox;

	Picto::CommandChannel* commandChannel;

	static QString filename;
	static QDir currentDir;
};

#endif