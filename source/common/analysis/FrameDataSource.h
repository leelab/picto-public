#ifndef _FRAME_DATA_SOURCE_H_
#define _FRAME_DATA_SOURCE_H_

#include <QSharedPointer>
#include <QScriptValue>
#include "AnalysisDataSource.h"
#include "FrameDataIterator.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API FrameDataSource : public AnalysisDataSource
#else
class FrameDataSource : public AnalysisDataSource
#endif
{
	Q_OBJECT
public:
	FrameDataSource();
	virtual ~FrameDataSource();
	static QSharedPointer<Asset> Create();

	//AnalysisDataSource specific functions
	virtual void restart();

	//Return the frame's AnalysisValue at the input time.
	virtual QSharedPointer<AnalysisValue> getValue(const EventOrderIndex& index);

public slots:
	double time(int triggerIndex);

protected:

	//Inherited
	virtual void recheckSessionData();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QSharedPointer<FrameDataIterator> frameIterator_;
	QSharedPointer<FrameData> lastDataUnit_;
	QSharedPointer<FrameData> latestValue_;
};
}; //namespace Picto
#endif