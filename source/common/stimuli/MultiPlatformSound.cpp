#include <QAudioDeviceInfo>
#include <QFile>
#include <qendian.h>
#include "MultiplatformSound.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Creates a MultiplatformSound object to play the audio file at the input path.
*/
MultiplatformSound::MultiplatformSound(QString path)
{
	sound_ = QSharedPointer<QSound>(new QSound(path));
	soundStarted_ = false;
}

void MultiplatformSound::play()
{
	sound_->play();
	soundStarted_ = true;
}
void MultiplatformSound::stop()
{
	sound_->stop();
}
/*! \copydoc PreloadedSound::setVolume()
 *	\attention Currently, the MultiplatformSound does not support
 *	volume changes.  This is why we have not yet added a setVolume()
 *	script function to the AudioElement class.
 */
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
/*! \copydoc PreloadedSound::volume()
 *	\attention Currently, the MultiplatformSound does not support
 *	volume changes.  This is why we have not yet added a setVolume()
 *	script function to the AudioElement class.  This function will
 *	always return 100%.
 */
int MultiplatformSound::volume()
{
	return 100;
}

bool MultiplatformSound::isReady()
{
	return isFileSupported(sound_->fileName());
}

QString MultiplatformSound::errorString()
{
	if(sound_->fileName().isEmpty())
		return "No file was loaded.";
	return "";
}

/*! \brief Creates a MultiplatformSound to play the audio file at the input path.
 * \details This function is meant to be used with PreloadedSound::setSoundConstructor()
 */
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

/*! \brief Checks if the file at the input path is supported by this PreloadedSound.
 *	\details This function uses code from the Qt Multimedia Spectrum app's wavFile.cpp
 * I didn't take a course in audio encoding protocols and work this all out on my own.
 */
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
