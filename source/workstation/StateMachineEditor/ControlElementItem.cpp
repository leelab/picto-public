#include "ControlElementItem.h"
#include "../../common/ControlElements/ControlElement.h"
ControlElementItem::ControlElementItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
WireableItem(editorState,contextMenu,asset)
{
	QSharedPointer<ControlElement> ctrlElem = asset.staticCast<ControlElement>();
	QList<QSharedPointer<Asset>> results = ctrlElem->getGeneratedChildren("Result");
	foreach(QSharedPointer<Asset> result,results)
	{
		addArrowSource(result->getName());
	}
	enableArrowDest();
}