#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <QUuid>
#include "AudioFileParameter.h"
#include "../memleakdetect.h"

namespace Picto {

QString AudioFileParameter::outputDir_("");
QSharedPointer<QFile> AudioFileParameter::lockFile_;

AudioFileParameter::AudioFileParameter()
: FileParameter()
{
	physicalFilePath_ = "";
	initTempOutputDir();
}

AudioFileParameter::~AudioFileParameter()
{
	if(!physicalFilePath_.isEmpty())
	{
		if(QFile::exists(physicalFilePath_))
			QFile::remove(physicalFilePath_);	}
}

QSharedPointer<Asset> AudioFileParameter::Create()
{
	return QSharedPointer<Asset>(new AudioFileParameter());
}

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

void AudioFileParameter::fileDataUpdated(const QByteArray& data)
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

QString AudioFileParameter::getSoundsDir()
{
	return outputDir_+QString("/sounds");
}

//Will create an empty ExpAudioFiles directory in the runpath once
//per executable run, clearing out any previous contents if there 
//were any.
void AudioFileParameter::initTempOutputDir()
{
	//We want to make sure that even if multiple Picto Workstations are open at the
	//same time, no one will remove the data that we are currently using.  We also 
	//want to erase the data if we are just opening for the first time and no one else
	//is using the data in the temporary folder.  For this reason, we attempt to remove
	//all files at the top level of the temporary directory, then if successful, remove
	//all other contained directories as well.  We then create a top level file in the
	//temporary directory and leave it open until the application closes.
	//If another workstation is open, we won't succeed in removing its open file so we
	//wont delete the temporary directory contents, otherwise, we will.
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
	Q_ASSERT(rc);

	//If the sounds sub directory doesn't exist, make it.
	QString soundsDir = getSoundsDir();
	if(!QFile::exists(soundsDir))
	{	//The directory doesn't exist yet.  Make it.
		QDir dir;
		dir.mkpath(soundsDir);
	}
}

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
