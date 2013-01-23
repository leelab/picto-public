#include <QtWidgets>
#include "BackgroundToolGroup.h"
#include "../../common/memleakdetect.h"


BackgroundToolGroup::BackgroundToolGroup(QSharedPointer<EditorState> editorState,QWidget *parent) :
	ToolGroup(editorState,parent)
{
	AddButton(tr("Blue Grid"),QIcon(":/icons/background1.png"));
	AddButton(tr("White Grid"),QIcon(":/icons/background2.png"));
	AddButton(tr("Gray Grid"),QIcon(":/icons/background3.png"));
	AddButton(tr("No Grid"),QIcon(":/icons/background4.png"));
}

void BackgroundToolGroup::doButtonAction(int buttonId)
{
	switch(buttonId)
	{
	case 0:
		getEditorState()->setBackgroundPattern(QPixmap(":/icons/background1.png"));
		break;
	case 1:
		getEditorState()->setBackgroundPattern(QPixmap(":/icons/background2.png"));
		break;
	case 2:
		getEditorState()->setBackgroundPattern(QPixmap(":/icons/background3.png"));
		break;
	case 3:
		getEditorState()->setBackgroundPattern(QPixmap(":/icons/background4.png"));
		break;
	}
}