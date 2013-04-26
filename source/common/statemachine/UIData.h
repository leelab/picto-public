#ifndef _UIData_H_
#define _UIData_H_

#include "../common.h"
#include "../parameter/AssociateRoot.h"

namespace Picto {

/*! \brief An analysis variable parameter.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API UIData : public AssociateRoot
#else
class UIData : public AssociateRoot
#endif
{
	Q_OBJECT

public:
	UIData();
	virtual ~UIData(){};
	static QSharedPointer<Asset> Create();
	virtual QString assetType(){return "UIData";};

	void setOpenAsset(int assetId);
	int getOpenAsset();

protected:
	virtual QString defaultTagName(){return "UIData";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QSharedPointer<AssetFactory> elementFactory_;


};


}; //namespace Picto

#endif
