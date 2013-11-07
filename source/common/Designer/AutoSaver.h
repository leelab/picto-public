#ifndef _AutoSaver_H_
#define _AutoSaver_H_

#include <QDir>
#include <QUuid>
#include <QSharedPointer>
#include <QWeakPointer>
#include "../common.h"

#include "../design/DesignRoot.h"

namespace Picto {

/*! \brief An analysis variable parameter.
 *
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
