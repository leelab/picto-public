#ifndef _SESSIONVERSIONINTERFACER_H_
#define _SESSIONVERSIONINTERFACER_H_
#include <QSharedPointer>
#include <QList>
#include <QHash>
#include <QString>
#include "../Experiment/Experiment.h"

namespace Picto {

struct SVIAssetNode;
/*! \brief Resets the Asset ID values of an Experiment to those used in a Session file.
 *	\details Since Session files could have been recorded at any time in the past, they may contain
 *	Designs that use an old version of the Picto Design Syntax.  In those cases, the Designs will be
 *	automatically upgraded, but this upgrade process can sometimes result in changes to Asset IDs of
 *	some of the Design Assets.  
 *
 *	For an example of this, consider a case where a particular type of Asset has become obsolete, so it is deleted 
 *	and the experiment is rebuilt from XML for an automatic upgrade.  While being rebuilt, there
 *	may be a Property whose default value was never changed in the design but whose value does change as part of
 *	the experimental run.  Since a new hole in the continous series of Asset IDs opened up due to the deletion
 *	of the ObsoleteAsset, this Property, which is generated automatically and does not appear in the design file
 *	since its value was never changed, takes on one of the Asset IDs of the freed up ObsoleteAsset.  Now, when 
 *	the value of that Property changes in the session file, the Playback system will have no way to know which
 *	Property was being referred to since the asset ID stored in the session file for the changed Property won't
 *	match up to a real Property in the design.
 *	
 *	This class takes care of that problem.  We simply construct it with a passed in pointer to the Experiment that
 *	we want to upgrade, then we add all Session Properties, Transitions, and Elements according to the data in the
 *	Session tables.  This is used internally to build up an Asset tree, then that Asset tree is traversed alongside
 *	the Experiment asset tree, and all of the Experiment Asset's AssetIDs are updated according to the corresponding ones from the
 *	session.  Any Properties, Transitions or Elements from the session that cannot be identified in the Experiment
 *	are added to an obsoleteAssets lookup table.  That lookup table contains all asset IDs for which no proper 
 *	Experimental Element can be found and therefore when changes come in for assetIds in the obsoleteAssets table,
 *	we know that we should just skip them before their corresponding Assets were found to be obsolete.  That table is
 *	accessible from getObsoleteAssets().
 *	
 *	\note Theoretically, the situation could come up at some point where a Property value from an obsolete asset needs
 *	to get mapped into a Property of a new Asset created to replace the obsolete one.  That hasn't come up yet, but when
 *	it does we will have to come up with a solution for that scenario.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class SessionVersionInterfacer
{
public:
	SessionVersionInterfacer(QSharedPointer<Experiment> initializedExperiment);
	virtual ~SessionVersionInterfacer();
	//Use this function to add properties with data from the loaded session file.
	//Returns an empty string on success and an error message on failure.
	QString addSessionProperty(QString name, int assetId, int parentAssetId);
	//Use this function to add transitions with data from the loaded session file.
	//Returns an empty string on success and an error message on failure.
	QString addSessionTransition(QString source, QString sourceResult, int assetId, int parentAssetId);
	//Use this function to add elements with data from the loaded session file.
	//Returns an empty string on success and an error message on failure.
	QString addSessionElement(int assetId, QString path);
	//Use this function once all assets have been added from the session file
	//to update the DesignConfig file such that asset ids in the session
	//file will be mapped to the correct assets in the DesignConfig object.
	//Returns an empty string on success and an error message on failure.
	QString updateSessionConfigFromSessionAssets();
	//Returns a lookup table that contains entries at all assetId indeces where
	//an asset was obsolete.  Assets with values in this table should not have
	//their properties updated by a session loader.
	QHash<int,bool> getObsoleteAssets();
	

private:	
	struct SVIAssetNode
	{
		SVIAssetNode(QString n,int id,int p,QString pathStr){name=n;assetId=id;parent=p;path=pathStr;isTransition=false;};
		QString name;
		int assetId;
		int parent;
		QString path;
		bool isTransition;
		QString transSource;
		QString transResult;
		QVector<int> children;
	};

	QString recurseResetAssetIds(QSharedPointer<Asset> asset,QSharedPointer<SVIAssetNode> node);
	void recurseAddObsoleteAssets(int assetId);
	QSharedPointer<Experiment> experiment_;
	QHash<int,QSharedPointer<SVIAssetNode>> nodesById_;
	QHash<QString,QSharedPointer<SVIAssetNode>> nodesByPath_;
	QHash<int,bool> obsoleteAssets_;
	QList<int> namelessIds_;
};

}; //namespace Picto
#endif