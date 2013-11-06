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
	static QSharedPointer<AutoSaver> create();
	virtual ~AutoSaver();

	virtual void setDesignRoot(QSharedPointer<DesignRoot> design);
	QSharedPointer<DesignRoot> takePreviouslyAutoSavedDesign();
public slots:
	void saveDesignToFile();
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
	QStringList savedDesignDirs_;
	QSharedPointer<QFile> lockFile_;	//File that is kept open while the application is active to prevent the directory from being deleted.
	QSharedPointer<QFile> designFile_;
	static QWeakPointer<AutoSaver> singleton_;
};


}; //namespace Picto

#endif
