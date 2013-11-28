#include "WireableResultItem.h"
#include "../../common/statemachine/Result.h"
#include "../../common/memleakdetect.h"
/*! \brief Constructs a new WireableResultItem.
 *	\details Inputs are passed directly to WireableItem::WireableItem.  
 *	The startbar is enabled using enableArrowDest().  Since this represents
 *	a result, it has no result bars.
 */
WireableResultItem::WireableResultItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
WireableItem(editorState,contextMenu,asset)
{
	QSharedPointer<Result> result = asset.dynamicCast<Result>();
	Q_ASSERT(!result.isNull());
	setName(result->getName());
	enableArrowDest();

	setRect(QRectF(QPointF(-19,-30),QPointF(19,30)));
	//I believe that the line below is now unnecessary since the DiagramItemFactory takes care of it... look into this.
	setSvgIcon(":/icons/result.svg");
}