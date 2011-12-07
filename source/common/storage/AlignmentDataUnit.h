#ifndef _ALIGNMENTDATAUNIT_H_
#define _ALIGNMENTDATAUNIT_H_
#include <QString>
#include <QList>

#include "../common.h"
#include "DataUnit.h"

namespace Picto {

/*!	\brief Unimplemented. Neural Data.
 *
 *	I'm not sure exactly how this was going to be used, but it is clearly
 *	intended to store neural data.  I suspect this may already be covered by
 *	the ACQGetCommand Handler in the Proxy Server.  It would be smart to move
 *	the definition here, but I didn't have time to do so...
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AlignmentDataUnit : public DataUnit
#else
class AlignmentDataUnit : public DataUnit
#endif
{
public:
	AlignmentDataUnit();
	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	void setTimestamp(double timestamp){timestamp_ = timestamp;}
	void setAlignCode(int alignCode){alignCode_ = alignCode;}
	void setAlignNumber(int alignNumber){alignNumber_ = alignNumber;}

	double getTimestamp(){return timestamp_;}
	int getAlignCode(){return alignCode_;}
	int hasAlignNumber(){return (alignNumber_ != -1);};
	int getAlignNumber(){return alignNumber_;}

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
double timestamp_;
int alignCode_;
int alignNumber_;
bool hasAlignNumber_;
};


}; //namespace Picto

#endif
