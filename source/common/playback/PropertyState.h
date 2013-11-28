#ifndef _PROPERTYSTATE_H_
#define _PROPERTYSTATE_H_
#include <QHash>
#include "DataState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
struct PropertyData;
/*! \brief Implements the DataState and PropertyReader classes to load a Picto Session database, 
 *	extract the data for Property value changes and implement functions for traversing through that data.
 *	\details The class is fairly simple, a QList of PropertyData objects is loaded from the 
 *	session data.  Each PropertyData represents a single Property value change and when moveToIndex() 
 *	is called, we just traverse through the list until we reach a PropertyData with the appropriate PlaybackIndex.
 *	Each time moveToIndex() causes us to pass through a PropertyData entry, the propertyChanged() 
 *	signal is called, which tells the rest of the playback system that a Property value changed.
 *
 *	\note Since the functions here simply implement the PropertyReader and DataState classes for
 *	data read in from a Session Database, there is not much to add in terms of documentation 
 *	beyond what was described above, so we will not be adding function level documentation
 *	for this class.
 *
 *	\note PropertyReader no longer has any contents since we realized that PropertyData was more naturally
 *	extracted as part of StateMachine playback instead of being gathered over time ranges.  We have left
 *	the PropertyReader interface extension here, but it could probably be removed.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class PropertyState : public PropertyReader, public DataState
{
	Q_OBJECT
public:
	PropertyState(bool forInitProperties = false);
	virtual void setDatabase(QSqlDatabase session);
	virtual void startRun(double runStartTime,double runEndTime = -1);
	virtual PlaybackIndex getCurrentIndex();
	virtual PlaybackIndex getNextIndex(double lookForwardTime);
	virtual void moveToIndex(PlaybackIndex index);
	/*! \brief Sets a lookup table of Asset Ids for Assets that have become obsolete.
	 *	\details When Property value changes for these Assets are encountered, they are ignored.
	 *	For more information, see the SessionVersionInterfacer object.  The SessionVersionInterfacer can
	 *	be used to prepare the input obsoleteAssetIds table.
	 */
	void setObsoleteAssets(QHash<int,bool> obsoleteAssetIds){obsoleteAssetIds_ = obsoleteAssetIds;};
signals:
	/*! \brief Emitted whenever a Property value change is traversed due to a call to moveToIndex().
	 *	\details The propId is the Asset Id of the Property, the value is the new value of the Property.
	*/
	void propertyChanged(int propId, QString value);

private:
	PlaybackIndex getNextIndex();
	PlaybackIndex globalToRunIndex(PlaybackIndex index);
	QSqlDatabase session_;
	QSharedPointer<QSqlQuery> query_;
	double runStart_;
	double runEnd_;
	int curr_;
	QList<PropertyData> data_;
	QHash<int,bool> obsoleteAssetIds_;
	bool forInitProperties_;
};

/*! \brief A struct used to store a single Property value change.
 *	\details Includes the PlaybackIndex of the Property value change, the Asset ID of the Property, and
 *	the new value.
 */
struct PropertyData
{
	PropertyData(){};
	PropertyData(PlaybackIndex index,int assetid,QString val){index_ = index;assetid_ = assetid;val_ = val;};
	PlaybackIndex index_;
	int assetid_;
	QString val_;
};

}; //namespace Picto
#endif