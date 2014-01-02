#ifndef _UIInfo_H_
#define _UIInfo_H_
#include <QUuid>

#include "../common.h"
#include "../storage/DataStore.h"

namespace Picto {

/*!	\brief This class used to hold information about the UI associated with each Asset (position of icon in canvas, etc)
 *	as part of the Experiment design tree.  Since then we have moved UI Data out into its own separate tree with UIData and UIElement classes
 *	rendering this class obsolete.  It should probably be deleted.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API UIInfo : public DataStore
#else
class UIInfo : public DataStore
#endif
{
	Q_OBJECT
public:
	UIInfo();
	virtual ~UIInfo(){};

	static QSharedPointer<Asset> Create();

	void setPos(QPoint pos);
	QPoint getPos();
	void setOpenDescendant(int assetId);
	int getOpenDescendant();
	virtual QString friendlyTypeName(){return "UI Info";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
