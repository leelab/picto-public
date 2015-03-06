#ifndef _BUFFER_FILE_GENERATOR_
#define _BUFFER_FILE_GENERATOR_

#include <QUuid>
#include <QFile>
#include <QSharedPointer>
#include "../common.h"

namespace Picto {

/*! \brief Creates temporary files in a temporary directory that is cleared everytime
 *	a new Pictoworkstation is opened so long as no other workstations were already open
 *	at the time.
 *	\details A "TemporaryFiles" directory is created in the Picto run directory and a 
 *	"lock file" is created and placed inside that isn't closed until the application closes.
 *	Each time a PictoWorkstation is opened, the temporary director is checked, if the lock
 *	files can be deleted, the files in the directory are deleted as well.  Otherwise 
 *	temporary files are simply added to the existing temporary directory.
 *	\note We need to use this type of set up in a few different places in Picto.  For
 *	playback video files, for AnalysisFileOutput files, and for design AutoSaved files.
 *	So far we have only used this BufferFileGenerator for the playback video files 
 *	(RecordingVisualTargetHost).  We should probably reuse it in all those places though.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
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