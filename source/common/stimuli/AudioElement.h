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

/*!	\brief Unimplemented. An audio stimuli
 *
 *	This is an unimplemented class that will eventually contain an audio element.
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AudioElement  : public OutputElement
#else
class AudioElement  : public OutputElement
#endif
{
	Q_OBJECT
	Q_PROPERTY(QString file READ getFile WRITE setFile)
public:
	AudioElement();
	virtual ~AudioElement(){};
	static QSharedPointer<Asset> Create();

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
