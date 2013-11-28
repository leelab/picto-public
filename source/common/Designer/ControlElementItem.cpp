#include "ControlElementItem.h"
#include "../../common/ControlElements/ControlElement.h"
#include "../../common/memleakdetect.h"
/*! \brief Constructs a new ControlElementItem.
 *	\details Inputs are passed directly to WireableItem::WireableItem.  A result bar is added
 *	for each of the underlying ControlElement's results.
 */
ControlElementItem::ControlElementItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
WireableItem(editorState,contextMenu,asset)
{
	QSharedPointer<ControlElement> ctrlElem = asset.staticCast<ControlElement>();
	QList<QSharedPointer<Asset>> results = ctrlElem->getGeneratedChildren("Result");
	foreach(QSharedPointer<Asset> result,results)
	{
		addResultArrowSource(result);
	}
}