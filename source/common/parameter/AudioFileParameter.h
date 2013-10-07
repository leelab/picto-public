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
 *	reloaded.
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
