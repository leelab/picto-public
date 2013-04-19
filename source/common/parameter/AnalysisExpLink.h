#ifndef _AnalysisExpLink_H_
#define _AnalysisExpLink_H_

#include <QSharedPointer>
#include "../common.h"
#include "../storage/asset.h"
#include "Parameter.h"
#include "../task/task.h"

namespace Picto {

/*! \brief An analysis variable parameter.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisExpLink : public Parameter
#else
class AnalysisExpLink : public Parameter
#endif
{
	Q_OBJECT

public:
	AnalysisExpLink();
	virtual ~AnalysisExpLink(){};
	static QSharedPointer<Asset> Create();

	QString getParentPath(){return propertyContainer_->getPropertyValue("ParentPath").toString();};
	int getParentId(){return propertyContainer_->getPropertyValue("ParentId").toInt();};
	void linkToAsset(QSharedPointer<Asset> asset);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QSharedPointer<Asset> linkedAsset_;
private slots:
	void updateLinkedAssetProperties();
};


}; //namespace Picto

#endif
