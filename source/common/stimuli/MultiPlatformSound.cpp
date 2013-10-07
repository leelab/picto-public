#include <QAudioDeviceInfo>
#include <QFile>
#include <qendian.h>
#include "MultiplatformSound.h"
#include "../memleakdetect.h"

namespace Picto {

MultiplatformSound::MultiplatformSound(QString path)
{
	//Check to see if the file is supported... there is no way to do this with QSound, but as a hack, we can use QMediaPlaylist
	//audioDecoder_.setSourceFilename(path);
	//if(audioDecoder_.error() != QAudioDecoder::NoError)
	//	errorMsg_ = audioDecoder_.errorString();
	//QAudioFormat invalidFormat;
	//invalidFormat.setCodec("audio/pcm");
	//audioDecoder_.setAudioFormat(invalidFormat);
	//audioDecoder_.start();
	sound_ = QSharedPointer<QSound>(new QSound(path));
	soundStarted_ = false;
}

void MultiplatformSound::play()
{
	sound_->play();
	//QSound::play(sound_->fileName());
	soundStarted_ = true;
}
void MultiplatformSound::stop()
{
	sound_->stop();
}
void MultiplatformSound::setVolume(int percent)
{

}

bool MultiplatformSound::playing()
{
	bool isFinished = sound_->isFinished();
	if(isFinished)
		soundStarted_ = false;
	return soundStarted_;
}

int MultiplatformSound::volume()
{
	return 100;
}

bool MultiplatformSound::isReady()
{
	//audioDecoder_.state();
	//audioDecoder_.stop();
	//if(!audioDecoder_.bufferAvailable())
	//{
	//	audioDecoder_.start();
	//	return "Audio not yet loaded";
	//}

	//if(audioDecoder_.error() != QAudioDecoder::NoError)
	//	errorMsg_ = audioDecoder_.errorString();
	//audioDecoder_.stop();
	//if(audioDecoder_.error() != QAudioDecoder::NoError)
	//	errorMsg_ = audioDecoder_.errorString();
	//QAudioFormat format = audioDecoder_.audioFormat();
	//QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
	//
	//if (!info.supportedCodecs().contains(format.codec()) || !info.supportedByteOrders().contains(format.byteOrder())) 
	//	qWarning()<<"raw audio format not supported by backend, cannot play audio.";
	return isFileSupported(sound_->fileName());


	return !sound_->fileName().isEmpty();
}

QString MultiplatformSound::errorString()
{
	if(sound_->fileName().isEmpty())
		return "No file was loaded.";
	return "";
}

QSharedPointer<PreloadedSound> MultiplatformSound::createMultiplatformSound(QString path)
{
	return QSharedPointer<MultiplatformSound>(new MultiplatformSound(path));
}



struct chunk
{
    char        id[4];
    quint32     size;
};

struct RIFFHeader
{
    chunk       descriptor;     // "RIFF"
    char        type[4];        // "WAVE"
};

struct WAVEHeader
{
    chunk       descriptor;
    quint16     audioFormat;
    quint16     numChannels;
    quint32     sampleRate;
    quint32     byteRate;
    quint16     blockAlign;
    quint16     bitsPerSample;
};

struct DATAHeader
{
    chunk       descriptor;
};

struct CombinedHeader
{
    RIFFHeader  riff;
    WAVEHeader  wave;
};

//Uses code from Qt Multimedia Spectrum app wavFile.cpp
bool MultiplatformSound::isFileSupported(QString path)
{
	QFile waveFile;
	QAudioFormat fileFormat;
	waveFile.setFileName(path);
	waveFile.open(QIODevice::ReadOnly);
	waveFile.seek(0);




	CombinedHeader header;
    bool result = waveFile.read(reinterpret_cast<char *>(&header), sizeof(CombinedHeader)) == sizeof(CombinedHeader);
    if (result) {
        if ((memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0
            || memcmp(&header.riff.descriptor.id, "RIFX", 4) == 0)
            && memcmp(&header.riff.type, "WAVE", 4) == 0
            && memcmp(&header.wave.descriptor.id, "fmt ", 4) == 0
            && (header.wave.audioFormat == 1 || header.wave.audioFormat == 0)) {

            // Read off remaining header information
            DATAHeader dataHeader;

            if (qFromLittleEndian<quint32>(header.wave.descriptor.size) > sizeof(WAVEHeader)) {
                // Extended data available
                quint16 extraFormatBytes;
                if (waveFile.peek((char*)&extraFormatBytes, sizeof(quint16)) != sizeof(quint16))
                    return false;
                const qint64 throwAwayBytes = sizeof(quint16) + qFromLittleEndian<quint16>(extraFormatBytes);
                if (waveFile.read(throwAwayBytes).size() != throwAwayBytes)
                    return false;
            }

            if (waveFile.read((char*)&dataHeader, sizeof(DATAHeader)) != sizeof(DATAHeader))
                return false;

            // Establish format
            if (memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0)
                fileFormat.setByteOrder(QAudioFormat::LittleEndian);
            else
                fileFormat.setByteOrder(QAudioFormat::BigEndian);

            int bps = qFromLittleEndian<quint16>(header.wave.bitsPerSample);
            fileFormat.setChannelCount(qFromLittleEndian<quint16>(header.wave.numChannels));
            fileFormat.setCodec("audio/pcm");
            fileFormat.setSampleRate(qFromLittleEndian<quint32>(header.wave.sampleRate));
            fileFormat.setSampleSize(qFromLittleEndian<quint16>(header.wave.bitsPerSample));
            fileFormat.setSampleType(bps == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);
        } else {
            return false;
        }
    }
	else
	{
		return false;
	}

	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
	if (!info.isFormatSupported(fileFormat))
		return false;
    return true;
}

}; //namespace Picto
