#ifndef _PreloadedSound_H_
#define _PreloadedSound_H_

#include <QSound>
#include <QSharedPointer>
#include "../common.h"

namespace Picto {

/*!	\brief Unimplemented. A Wave Sound class.
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API PreloadedSound
#else
class PreloadedSound
#endif
{
public:
	virtual ~PreloadedSound(){};
	virtual void play() = 0;
	virtual void stop() = 0;
	virtual void setVolume(int percent) = 0;
	virtual bool playing() = 0;
	virtual int volume() = 0;
	virtual bool isReady() = 0;
	virtual QString errorString() = 0;

	static void setSoundConstructor(QSharedPointer<PreloadedSound> (*constructFunc)(QString));
	static QSharedPointer<PreloadedSound> createSound(QString path);

private:
	static QSharedPointer<PreloadedSound> (*constructFunc_)(QString);
};


}; //namespace Picto

#endif
