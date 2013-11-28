#ifndef _AutoSaver_H_
#define _AutoSaver_H_

#include <QDir>
#include <QUuid>
#include <QSharedPointer>
#include <QWeakPointer>
#include "../common.h"

#include "../design/DesignRoot.h"

namespace Picto {

/*! \brief Handles automatically saving the current Picto Design at a defined interval and restoring previously Auto-Saved files.
 *	\details The auto-saved file is saved in a subdirectory of the AutoSave subdirectory of the Picto installation
 *	folder.  The auto-saved file is saved along with a "lock file." When the AutoSaver is deleted, both files and their
 *	directory are removed.  This means that future AutoSaver objects can check for auto-saved files by looking through the
 *	subdirectories of [PictoInstallDir]/AutoSave and finding all of the files whose neighboring LockFile doesn't exist or
 *	can be deleted (since it can't be deleted when open in another workstation).
 *
 *	Restoring an Auto-Saved file is performed though the use of an AutoSaveWidget generated by an AutoSaver.  The AutoSaver
 *	and AutoSaveWidget classes together form the entire Auto-Save system.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AutoSaver : public QObject
#else
class AutoSaver : public QObject
#endif
{
	Q_OBJECT

public:
	static QSharedPointer<AutoSaver> getSingleton();
	virtual ~AutoSaver();

	void start(int savePeriodMs);
	void stop();
	virtual void setDesignRoot(QSharedPointer<DesignRoot> design,bool saveFirstVersion);
	//If the system saves a file, it can call this to remove the superfluous
	//autosaved file.
	void removeFileUntilNextChange();
	QDialog* getDesignRestoreDialog(QWidget* parent);
signals:
	void openDesign(QSharedPointer<DesignRoot> designRoot);
protected:

	QString getAutoSaveDir();

private:
	AutoSaver();
	void initAutoSaveDir();
	void removeFilesThenDirectories(QDir container);
	QString buildDesignFileName(const QString designName);
	QSharedPointer<DesignRoot> designRoot_;
	QString latestDesignCode_;
	QString autoSaveDir_;
	QList<QDir> savedDesignDirs_;
	QSharedPointer<QFile> lockFile_;	//File that is kept open while the application is active to prevent the directory from being deleted.
	QSharedPointer<QFile> designFile_;
	QTimer saveTimer_;
	static QWeakPointer<AutoSaver> singleton_;
private slots:
	void saveDesignToFile();
	void deleteRestorableDesignFile(QString filePath);
	void restoreDesignFile(QString filePath);
};


}; //namespace Picto

#endif
