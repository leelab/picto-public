#include <QtGui>
#include <QLayoutIterator>
#include "ToolGroup.h"


ToolGroup::ToolGroup(QWidget *parent) :
	QWidget(parent)
{
	buttonGroup_ = new QButtonGroup;
	buttonGroup_->setExclusive(false);
	connect(buttonGroup_, SIGNAL(buttonClicked(int)),this, SLOT(buttonGroupClicked(int)));
	connect(buttonGroup_, SIGNAL(buttonReleased(int)),this, SLOT(buttonGroupReleased(int)));
	layout_ = new QGridLayout;
	setLayout(layout_);
	clearButtons();
}

void ToolGroup::AddButton(const QString &label, QIcon icon)
{
    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
	int id=(2*row_)+col_;
    buttonGroup_->addButton(button,id);
	QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(label), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
	layout_->addWidget(widget,row_,col_);
	widgets_.push_back(widget);
	nameMap_[id] = label;
	int maxRow = row_;
	row_ = (col_==1)?row_+1:row_;
	col_ = (col_==1)?col_-1:col_+1;
	int maxCol = (row_>0)?1:0;

	for(int i=0;i<=maxRow;i++)
		layout_->setRowStretch(i,0);
	layout_->setRowStretch(maxRow+1,10);
	for(int i=0;i<=maxCol;i++)
		layout_->setColumnStretch(i,0);
	layout_->setColumnStretch(maxRow+1,10);

}

void ToolGroup::clearButtons()
{
	foreach(QWidget *widget,widgets_)
	{
		layout_->removeWidget(widget);
		delete widget;
	}
	widgets_.clear();
	nameMap_.clear();
	row_ = 0;
	col_ = 0;
}

void ToolGroup::buttonGroupClicked(int)
{
	int id = buttonGroup_->checkedId();
    QList<QAbstractButton *> buttons = buttonGroup_->buttons();
    foreach (QAbstractButton *button, buttons) 
	{
		if (buttonGroup_->button(id) != button)
			button->setChecked(false);
    }
	if(nameMap_.contains(id))
		selectedButton_ = nameMap_[id];
	else
		selectedButton_ = "";
	emit insertionItemSelected(selectedButton_);
}

void ToolGroup::buttonGroupReleased(int)
{
	selectedButton_ = "";
	emit insertionItemSelected(selectedButton_);
}