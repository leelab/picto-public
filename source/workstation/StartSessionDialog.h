#ifndef _START_SESSION_DIALOG_H_
#define _START_SESSION_DIALOG_H_

#include <QDialog>
#include <QDir>
#include <QMap>
#include <QUuid>

#include "../common/network/CommandChannel.h"
#include "../common/experiment/experiment.h"

class QToolButton;
class QComboBox;
class QLabel;
class QLineEdit;
class QDialogButtonBox;

/*!	\brief Deprecated. A dialog for starting a session
 *
 *	This was originally used as a dialog to start up a new session.  We no longer use 
 *	this dialog, and hence the code is no longer needed.  However, I have left it in place
 *	as it shows exactly how to start up a new session.
 */
class StartSessionDialog : public QDialog
{
	Q_OBJECT
public:
	StartSessionDialog(QSharedPointer<Picto::CommandChannel> commandChannel);

	QString getExperimentFile() { return currentDir_.absoluteFilePath(filename_); };
	QString getDirectorID();
	QSharedPointer<Picto::Experiment> getExperiment() { return experiment_; }
	QUuid getSessionId() { return sessionId_; };

private slots:
	void selectExperimentFile();
	void loadExperiment();

private:
	typedef struct
	{
		QString name;
		QString status;
		QString addr;
		QString id;
	}ComponentInstance;


	void createDialog();
	QComboBox* createDirectorList();

	QLabel *fileLabel_;
	QLabel *directorLabel_;
	QLineEdit *fileEdit_;
	QToolButton *moreFilesButton_;
	QComboBox *directorComboBox_;
	QDialogButtonBox *buttonBox_;

	QSharedPointer<Picto::CommandChannel> commandChannel_;

	static QString filename_;
	static QDir currentDir_;

	QList<ComponentInstance> directors_;
	QSharedPointer<Picto::Experiment> experiment_;

	QUuid sessionId_;
};

#endif