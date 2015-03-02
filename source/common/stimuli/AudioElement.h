#ifndef _AUDIOELEMENT_H_
#define _AUDIOELEMENT_H_

#include <QSharedPointer>
#include <QHash>
#include "../stimuli/PreloadedSound.h"
#include "OutputElement.h"
#include "../compositor/mixingsample.h"
#include "../parameter/FileParameter.h"
#include "../common.h"

namespace Picto {

/*!	\brief An Audio Stimulus used to play sounds in Picto.
 *	\details This is an Audio OutputElement.  It is used to play sounds during the course of a Task run.
 *	It includes an AudioFile Property that is used to reference an AudioFileParameter which includes the actual
 *	audio file that will be played.  This indirection allows us to switch the AudioFile associated with an AudioElement
 *	during the course of an experiment.  To play or stop the sound from the AudioFileParameter, just use the play() or 
 *	stop() script functions.  These will cause the AudioFile to start or stop playing at the time of the next presented
 *	frame.  To check if an AudioElement's sound is currently playing, use the isPlaying() script function.  
 *
 *	Since the AudioElement needs to Cache the sounds that it is going to play, but it does not know at run time if a new sound
 *	file will be set to it or not at runtime, the cacheFile() script function can be used at the beginning of a run to assure that
 *	there will be no lag in the middle of the run when the AudioElement tries to play an AudioFileParameter's sound for the first time.
 *	\note If play() is called immediately followed by isPlaying(), isPlaying() will return false.  This is because isPlaying()
 *	records whether or not the Audio is actually playing through the speakers, and the audio doesn't start actually being played
 *	until the frame presentation time after play() is called.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AudioElement  : public OutputElement
#else
class AudioElement  : public OutputElement
#endif
{
	Q_OBJECT
	/*! \brief Sets/gets the name of the AudioFile element from which this AudioElement will play audio.*/
	Q_PROPERTY(QString file READ getFile WRITE setFile)
public:
	AudioElement();
	virtual ~AudioElement(){};
	static QSharedPointer<Asset> Create();
	static const QString type;

	void addMixingSample(QSharedPointer<MixingSample> mixingSample);
	QSharedPointer<MixingSample> getMixingSample(QString sampleType);

	QString getFile();
	void setFile(QString fileObjectName);

	virtual QString friendlyTypeName(){return "Audio Element";};
	virtual QString getUITemplate(){return "AudioElement";};

public slots:
	void play();
	void stop();
	bool isPlaying();
	void cacheFile(QString fileElementName);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual void scriptableContainerWasReinitialized();
	virtual bool executeSearchAlgorithm(SearchRequest searchRequest);
private:
	QSharedPointer<PreloadedSound> getSound();
	void updateFileObject(QString fileElementName);
	void doSoundCommands();
	enum SOUND_COMMAND{NONE,PLAY,STOP};
	SOUND_COMMAND currCommand_;
	QWeakPointer<FileParameter> fileObj_;
	QHash<QString, QSharedPointer<MixingSample> > mixingSamples_;
	QHash<QString, QSharedPointer<PreloadedSound>> cachedSounds_;
private slots:
	void audioFileChanged(Property*,QVariant);
	void commandChanged(Property*,QVariant);
	void playOccured(QString mixSampleName);
	void stopOccured(QString mixSampleName);
};


}; //namespace Picto

#endif
