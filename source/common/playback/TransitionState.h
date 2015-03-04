#ifndef _TRANSITIONSTATE_H_
#define _TRANSITIONSTATE_H_
#include <QHash>
#include "DataState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
struct PlaybackTransData;
/*! \brief Implements the DataState and TransitionReader classes to load a Picto Session database, 
 *	extract transition traversal data and implement functions for traversing through that data.
 *	\details The class is fairly simple, a QList of PlaybackTransData objects is loaded from the 
 *	session data.  Each PlaybackTransData represents a single Transition traversal and when moveToIndex() 
 *	is called, we just move through the list until we reach a PlaybackTransData with the appropriate PlaybackIndex.
 *	Each time moveToIndex() causes us to pass through a PlaybackTransData entry, the transitionActivated() 
 *	signal is called, which tells the rest of the playback system that a Transition was traversed.
 *
 *	\note Since the functions here simply implement the TransitionReader and DataState classes for
 *	data read in from a Session Database, there is not much to add in terms of documentation 
 *	beyond what was described above, so we will not be adding function level documentation
 *	for this class.

 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class TransitionState : public QObject, public DataState
{
	Q_OBJECT
public:
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
	int getFirstTransIdInRun();
signals:
	/*! \brief Emitted whenever a Transition traversal is passed over due to a call to moveToIndex().
	 *	\details The transId is the Asset Id of the Transition that was traversed.
	 */
	void transitionActivated(int transId);
private:
	PlaybackIndex getNextIndex();
	PlaybackIndex globalToRunIndex(PlaybackIndex index);
	QSqlDatabase session_;
	QSharedPointer<QSqlQuery> query_;
	double runStart_;
	double runEnd_;
	int curr_;
	int firstLocationInRun_;
	QList<PlaybackTransData> data_;
	QHash<int,bool> obsoleteAssetIds_;
};

/*! \brief A struct used to store a single Transition traversal.
 *	\details Includes the PlaybackIndex of the Transition traversal, and the Asset Id (transId_) of the
 *	Transition that was traversed.
 */
struct PlaybackTransData
{
	PlaybackTransData(){};
	PlaybackTransData(PlaybackIndex index,int transId){index_ = index;transId_ = transId;};
	PlaybackIndex index_;
	int transId_;
};

}; //namespace Picto
#endif