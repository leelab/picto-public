#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <QUuid>
#include "AudioFileParameter.h"
#include "../memleakdetect.h"

namespace Picto {

QString AudioFileParameter::outputDir_("");
QSharedPointer<QFile> AudioFileParameter::lockFile_;

/*! \brief Creates a new AudioFileParameter.
 *	\details Calls initTempOutputDir() in the process to assure that a directory is available for temporary audio files.
 */
AudioFileParameter::AudioFileParameter()
: FileParameter()
{
	physicalFilePath_ = "";
	initTempOutputDir();
}

/*! \brief Attempts to remove any temporary audio file created by this object from disk before the object is deleted.
 *	\details If this attempt fails, it should end up getting deleted in a future AudioFileParameter initialization anyway.
 */
AudioFileParameter::~AudioFileParameter()
{
	if(!physicalFilePath_.isEmpty())
	{
		if(QFile::exists(physicalFilePath_))
			QFile::remove(physicalFilePath_);	}
}

/*! \brief Creates a new AudioFileParameter object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> AudioFileParameter::Create()
{
	return QSharedPointer<Asset>(new AudioFileParameter());
}

/*! \brief Returns a shared pointer to a PreloadedSound object for this AudioFileParameter's file.
 *	\details Since we want multiple AudioElements to be able to play a sound from the same AudioFileParameter
 *	separately, we need to create a new PreloadedSound every time this is called.  Since we have found that there is sometimes a lag
 *	until the file can be used however, we always keep around one more PreloadedSound than necessary.  When this function is called, we
 *	create a new one, but return the one that we had previously created.
 */
QSharedPointer<PreloadedSound> AudioFileParameter::createSound()
{
	QSharedPointer<PreloadedSound> buffer = sound_;
	sound_ = PreloadedSound::createSound(physicalFilePath_);
	return buffer;
}

void AudioFileParameter::postDeserialize()
{
	FileParameter::postDeserialize();
}

/*! \brief Extends FileParameter::validateObject() to verify that this AudioFileParameter's file is a valid audio file.
*/
bool AudioFileParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!FileParameter::validateObject(xmlStreamReader))
		return false;
	QByteArray fileData = getFileData();
	if(fileData.isEmpty())
	{
		addError("Audio file was empty or could not be loaded.");
		return false;
	}
	if(!sound_)
	{	//If the file isn't ready yet, there must be a problem
		addError("Could not load sound.");
		return false;
	}
	if(!sound_->isReady())
	{	//If the file isn't ready yet, there must be a problem
		addError("Could not load sound: " + sound_->errorString());
		return false;
	}

	return true;
}

/*! \brief Called when this AudioFileParameter's file data changes.  Writes the data to a temporary file and loads
 *	a PreloadedSound from that file for later usage by an AudioElement.
 *	\note We create a physical file rather than simply using the file's binary data since many sound implementations
 *	that are available for preuse (ie. the ones underlying the PreloadedSound class) require an actual physical file.
 */
void AudioFileParameter::fileDataUpdated(const QByteArray&)
{
	physicalFilePath_ = "";
	QString filePath = getSoundsDir()+"/"+QUuid::createUuid().toString();
	if(QFile::exists(filePath))
		QFile::remove(filePath);
	QFile file_(filePath);
	//Open file
	if(!file_.open(QIODevice::WriteOnly))
	{
		//Couldn't open the file for some reason, leave physicalFilePath_ empty
		return;
	}
	file_.write(getFileData());
	file_.close();
	physicalFilePath_ = filePath;

	sound_ = PreloadedSound::createSound(physicalFilePath_);
}

/*! \brief Returns a path to the directory where temporary audio files should go.
 */
QString AudioFileParameter::getSoundsDir()
{
	return outputDir_+QString("/sounds");
}

/*!	\brief Creates an empty ExpAudioFiles directory in the runpath once
 *	per executable run, clearing out any previous contents if there 
 *	were any.
 *	\details We want to make sure that even if multiple Picto applications are open at the
 *	same time, no one will remove the data that we are currently using.  We also 
 *	want to erase the data if we are just opening for the first time and no one else
 *	is using the data in the ExpAudioFiles folder.  For this reason, we attempt to remove
 *	all files at the top level of the ExpAudioFiles directory, then if successful, remove
 *	all other contained directories as well.  We then create a top level file in the
 *	temporary directory and leave it open until the application closes.
 *	If another workstation is open, we won't succeed in removing its open file so we
 *	wont delete the ExpAudioFiles directory contents, otherwise, we will.
 */
void AudioFileParameter::initTempOutputDir()
{
	if(!outputDir_.isEmpty())
		return;
	QDir dir(QCoreApplication::applicationDirPath()+"/../");
	dir.mkdir("ExpAudioFiles");
	dir.cd("ExpAudioFiles");
	removeFilesThenDirectories(dir);

	outputDir_ = dir.absolutePath();

	QString fileName = outputDir_+"/"+QUuid::createUuid().toString();
	lockFile_ = QSharedPointer<QFile>(new QFile(fileName));
	bool rc = lockFile_->open(QIODevice::WriteOnly | QIODevice::Text);
	IGNORED_PARAMETER(rc);
	Q_ASSERT(rc);

	//If the sounds sub directory doesn't exist, make it.
	QString soundsDir = getSoundsDir();
	if(!QFile::exists(soundsDir))
	{	//The directory doesn't exist yet.  Make it.
		QDir dir;
		dir.mkpath(soundsDir);
	}
}
/*! \brief Removes everything in the input directory and all of its sub directories.
 *	\details Stops at any level where a file cannot be removed.
 */
void AudioFileParameter::removeFilesThenDirectories(QDir container)
{
	QStringList files = container.entryList(QDir::Files);
	foreach(QString fileName,files)
	{
		if(!container.remove(fileName))
		{
			return;
		}
	}
	QStringList directories = container.entryList(QDir::Dirs|QDir::NoDotAndDotDot|QDir::NoSymLinks);
	foreach(QString directory,directories)
	{
		if(!container.rmdir(directory))
		{
			QDir subDir = container;
			subDir.cd(directory);
			removeFilesThenDirectories(subDir);
			container.rmdir(directory);
		}
	}
}

}; //namespace Picto
