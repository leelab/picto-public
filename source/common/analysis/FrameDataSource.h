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

	virtual QString getUITemplate(){return "FrameDataSource";};

protected:

	//Inherited
	virtual QSharedPointer<AnalysisDataIterator> createDataIterator();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};
}; //namespace Picto
#endif