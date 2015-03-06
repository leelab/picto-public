#ifndef _UIData_H_
#define _UIData_H_

#include "../common.h"
#include "../parameter/AssociateRoot.h"

namespace Picto {

/*! \brief The container of all UI information for a particular AssociateRootHost (Experiment or Analysis).
 *	\details Some data associated with Picto Experiment and Analysis designs does not affect functionality of 
 *	the Experiment or Analysis, but needs to be saved in order for the StateMachine Designer UI to be usable.
 *	This includes things like the position of a particular Asset's icon in the canvas, notes about the function
 *	of a particular Asset and the id of the Asset that was last open in the framework.  UIData objects
 *	store all of this data in a data tree that is separate from the actual Experiment and Analysis trees so that
 *	essential information does not get mixed in with non-essential UI information.  Global data like the Id of the open Asset 
 *	is stored in the UIData object itself,.  Asset specific data is stored in child UIElement classes.  
 *
 *	The code handling Linkage to the Analysis and Experiment (AssociateRootHost) trees is handled by the inherited AssociateRoot class.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
	virtual QString friendlyTypeName(){return "UI Data";};

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
