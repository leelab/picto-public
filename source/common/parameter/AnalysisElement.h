#ifndef _AnalysisElement_H_
#define _AnalysisElement_H_

#include <QString>
#include <QUuid>
#include "../storage/Asset.h"
#include "Analysis.h"
#include "AnalysisExpLink.h"
#include "../common.h"

namespace Picto {

//The #defines below implements the AnalysisElement for its child classes in a default way.
//This is really just a simple way of cleaning up the multiple inheritance necessary
//to all AnalysisElements to inherit from design elements but still have common properties.
#define ANALYSIS_ELEMENT_IMPLEMENTATION 	virtual QString getParentPath()	\
											{								\
												QSharedPointer<AnalysisExpLink> lnk = getGeneratedChildren("ExpLink").first().staticCast<AnalysisExpLink>();	\
												return lnk->getParentPath();\
											};	\
											\
											virtual int getParentId()	\
											{								\
												QSharedPointer<AnalysisExpLink> lnk = getGeneratedChildren("ExpLink").first().staticCast<AnalysisExpLink>();	\
												return lnk->getParentId();\
											};	\
											virtual void linkToAsset(QSharedPointer<Asset> asset)	\
											{								\
												QSharedPointer<AnalysisExpLink> lnk = getGeneratedChildren("ExpLink").first().staticCast<AnalysisExpLink>();	\
												lnk->linkToAsset(asset);	\
											};	\
											virtual AttachmentResult attachToLinkedAsset(bool tryLinkingById)	\
											{	\
												QSharedPointer<Asset> analysis = getParentAsset();	\
												Q_ASSERT(analysis);	\
												QSharedPointer<Task> linkedTask = analysis.staticCast<Analysis>()->getLinkedTask();	\
												Q_ASSERT(linkedTask);	\
												\
												if(tryLinkingById)	\
												{	\
													if(linkedTask->AddAnalysisChild(getAnalysisId(),getParentId(),identifier(),selfPtr()))	\
														return SUCCESS_BY_LINK;	\
												}	\
												QString pathWithTaskName = linkedTask->getName() + "::" + getParentPath();	\
												if(linkedTask->AddAnalysisChild(getAnalysisId(),pathWithTaskName,identifier(),selfPtr()))	\
													return SUCCESS_BY_PATH;	\
												return FAIL;	\
											};	\
											virtual QUuid getAnalysisId()	\
											{								\
												QSharedPointer<Asset> analysis = getParentAsset();	\
												Q_ASSERT(analysis);	\
												return analysis.staticCast<Analysis>()->getAnalysisId();	\
											};	\
											virtual QString getInfoString()	\
											{	\
												return "Name: " + getName() + "\nType: " + identifier() + "\nLinked to: " + getParentPath();	\
											};

#define EXP_LINK_FACTORY_CREATION AddDefinableObjectFactory("ExpLink",QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(AnalysisExpLink::Create))));

/*! \brief Interface to Analysis Elements
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisElement
#else
class AnalysisElement
#endif
{

public:
	AnalysisElement(){};
	virtual ~AnalysisElement(){};

	virtual QString getParentPath() = 0;
	virtual int getParentId() = 0;
	virtual void linkToAsset(QSharedPointer<Asset> asset) = 0;
	enum AttachmentResult { SUCCESS_BY_LINK,SUCCESS_BY_PATH,FAIL };
	virtual AttachmentResult attachToLinkedAsset(bool tryLinkingById) = 0;
	virtual QUuid getAnalysisId() = 0;
	virtual QString getInfoString() = 0;

private:
	QUuid analysisId_;
};


}; //namespace Picto

#endif
