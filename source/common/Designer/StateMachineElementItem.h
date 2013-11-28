#ifndef STATEMACHINEELEMENTITEM_H
#define STATEMACHINEELEMENTITEM_H

#include "WireableItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

/*! \brief A WireableItem that represents a StateMachineElement.
 *	\details Essentially this is just a WireableItem that has a start bar and result bars for each child results.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class StateMachineElementItem : public WireableItem
{
	Q_OBJECT
public:
	 StateMachineElementItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
	virtual ~StateMachineElementItem(){};
};


#endif
