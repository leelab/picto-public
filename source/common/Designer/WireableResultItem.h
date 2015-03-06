#ifndef WIREABLERESULTITEM_H
#define WIREABLERESULTITEM_H

#include "WireableItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

/*! \brief A WireableItem that represents a Result.
 *	\details Essentially this is just a WireableItem with a start bar and no result bars.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class WireableResultItem : public WireableItem
{
	Q_OBJECT
public:
	 WireableResultItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
	 virtual ~WireableResultItem(){};

};


#endif
