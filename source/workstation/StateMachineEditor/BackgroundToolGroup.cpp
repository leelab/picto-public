#include <QtGui>
#include "BackgroundToolGroup.h"


BackgroundToolGroup::BackgroundToolGroup(QWidget *parent) :
	ToolGroup(parent)
{
	AddButton(tr("Blue Grid"),QIcon(":/icons/background1.png"));
	AddButton(tr("White Grid"),QIcon(":/icons/background2.png"));
	AddButton(tr("Gray Grid"),QIcon(":/icons/background3.png"));
	AddButton(tr("No Grid"),QIcon(":/icons/background4.png"));
}