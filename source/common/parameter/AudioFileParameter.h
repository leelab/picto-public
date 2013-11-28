#ifndef _AudioFileParameter_H_
#define _AudioFileParameter_H_

#include <QImage>
#include <QDir>
#include <QMediaPlayer>
#include "../common.h"
#include "../stimuli/PreloadedSound.h"
#include "FileParameter.h"

namespace Picto {

/*! \brief An Audio File parameter.
 *
 *	This parameter holds an audio file.  Not the path to the file, but the actual file such that changing
 *	the original file on disk will not affect the contents of this parameter unless the file is
 *	reloaded.  Internally, this class actually creates a new temporary file whenever the file data in the FileParameter
 *	changes.  This is for compatibility with sound classes that require an actual physical file for playback.  Whenever
 *	the File set to the FileParameter changes, an new underlying temporary file will be created and used.  
 *
 *	This class works with the AudioElement class such that multiple AudioElement objects can use the same
 *	AudioFileParameter.  Each one will still represent a different sound though, and if multiple AudioElements
 *	reference the same AudioFileParameter and are in scope at the same time.  If they play the audio file at different
 *	times, both sounds will be mixed through the speakers with their own separate timing as if they were two entirely different sound
 *	objects.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AudioFileParameter : public FileParameter
#else
class AudioFileParameter : public FileParameter
#endif
{
	Q_OBJECT

public:
	AudioFileParameter();
	virtual ~AudioFileParameter();

	static QSharedPointer<Asset> Create();

	virtual QString getUITemplate(){return "AudioFileParameter";};
	virtual QString friendlyTypeName(){return "Audio File";};
	QSharedPointer<PreloadedSound> createSound();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual void fileDataUpdated(const QByteArray& data);
private:
	bool wasReset_;
	QString physicalFilePath_;
	QSharedPointer<PreloadedSound> sound_;
	static QString getSoundsDir();
	static void initTempOutputDir();
	static void removeFilesThenDirectories(QDir container);
	static QString outputDir_;
	static QSharedPointer<QFile> lockFile_;	//File that is kept open while the application is active to prevent the directory from being deleted.
};


}; //namespace Picto

#endif
