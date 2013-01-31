#ifndef _BUFFER_FILE_GENERATOR_
#define _BUFFER_FILE_GENERATOR_

#include <QUuid>
#include <QFile>
#include <QSharedPointer>
#include "../common.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API BufferFileGenerator
#else
class BufferFileGenerator
#endif
{
public:
	static QSharedPointer<QFile> createTempFile(QString subDirName, QString fileType);

private:
	static QString createSubDirectory(QString subDirName);
	static void initTempOutputDir();
	static void removeFilesThenDirectories(QDir container);
	static QString outputDir_;
	static QSharedPointer<QFile> lockFile_;	//File that is kept open while the application is active to prevent the directory from being deleted.
};
}; //namespace Picto
#endif