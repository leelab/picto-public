#ifndef _SESSIONVERSIONINTERFACER_H_
#define _SESSIONVERSIONINTERFACER_H_
#include <QSharedPointer>
#include <QList>
#include <QHash>
#include <QString>
#include "../Experiment/Experiment.h"

namespace Picto {

struct SVIAssetNode;
/*! \brief Holds Data State's Data.
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
	//to update the ExperimentConfig file such that asset ids in the session
	//file will be mapped to the correct assets in the ExperimentConfig object.
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