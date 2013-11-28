#ifndef CONTROLELEMENTITEM_H
#define CONTROLELEMENTITEM_H
#include "WireableItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

/*! \brief A WireableItem that represents a ControlElement.
 *	\details Essentially this is just a WireableItem that has not start bar and has result bars for each of its results.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class ControlElementItem : public WireableItem
{
	Q_OBJECT
public:
	 ControlElementItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
	 virtual ~ControlElementItem(){};
private:

};


#endif
