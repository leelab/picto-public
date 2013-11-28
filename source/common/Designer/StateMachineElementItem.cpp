#include "StateMachineElementItem.h"
#include "../../common/StateMachine/StateMachineElement.h"
#include "../../common/memleakdetect.h"
/*! \brief Constructs a new StateMachineElementItem.
 *	\details Inputs are passed directly to WireableItem::WireableItem.  A result bar is added
 *	for each of the underlying ControlElement's results.  The startbar is enabled using enableArrowDest().
 */
StateMachineElementItem::StateMachineElementItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
WireableItem(editorState,contextMenu,asset)
{
	QSharedPointer<StateMachineElement> stateMachElem = asset.staticCast<StateMachineElement>();
	QList<QSharedPointer<Asset>> results = stateMachElem->getGeneratedChildren("Result");
	foreach(QSharedPointer<Asset> result,results)
	{
		addResultArrowSource(result);
	}
	enableArrowDest();

	//Set default size and icon
	setRect(QRectF(QPointF(-19,-30),QPointF(19,30)));
	//I believe that the line below is now unnecessary since the DiagramItemFactory takes care of it... look into this.
	setSvgIcon(":/icons/statemachineelement.svg");
}