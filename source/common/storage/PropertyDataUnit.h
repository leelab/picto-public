#ifndef _PROPERTYDATAUNIT_H_
#define _PROPERTYDATAUNIT_H_

#include "../common.h"
#include "DataUnit.h"

#include <QList>
#include <QMap>

namespace Picto {

/*!	\brief Stores data about a particular change to a Property's value.
 *
 *	\details This class holds data about a single Property value change.  The Property whose
 *	value changed is referenced by its AssetId.  The changed value can be either a runValue or
 *	an initValue, as indicated by the initValue_ field.  The Value itself is stored in a string
 *	form that can be converted to a Property value by using Property::valFromUserString().
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API PropertyDataUnit : public DataUnit
#else
class PropertyDataUnit : public DataUnit
#endif
{
public:
	PropertyDataUnit();
	PropertyDataUnit(int index, bool initValue, QString value);

	/*! \brief Sets the FrameId of the next frame to be presented after the Property value change occurs.*/
	void setActionFrame(qulonglong frameId){actionFrame_ = frameId;};
	/*! \brief Returns the FrameId of the next frame to be presented after the Property value change occurs.*/
	qulonglong getActionFrame(){return actionFrame_;};
	
	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
		
	int index_;					//!< The AssetId of the Property whose value changed.
	bool initValue_;			//!< True if the change was to an initValue, false if its a runValue.
	QString value_;				//!< The new Property value.
	qulonglong actionFrame_;	//!< The FrameId of the next Frame to be presented after the Property change takes place.

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);


};


}; //namespace Picto

#endif
