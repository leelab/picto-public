#include "ControlElementItem.h"
#include "../../common/ControlElements/ControlElement.h"
#include "../../common/memleakdetect.h"
ControlElementItem::ControlElementItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QMenu* scriptContextMenu, QSharedPointer<Asset> asset) :
WireableItem(editorState,contextMenu,scriptContextMenu,asset)
{
	QSharedPointer<ControlElement> ctrlElem = asset.staticCast<ControlElement>();
	QList<QSharedPointer<Asset>> results = ctrlElem->getGeneratedChildren("Result");
	foreach(QSharedPointer<Asset> result,results)
	{
		addArrowSource(result);
	}
}