#include <QtWidgets>
//#include <QLayoutIterator>
#include "ToolGroup.h"
#include "../../common/memleakdetect.h"


ToolGroup::ToolGroup(QSharedPointer<EditorState> editorState, QWidget *parent) :
	editorState_(editorState),
	QWidget(parent)
{
	buttonGroup_ = QSharedPointer<QButtonGroup>(new QButtonGroup);
	buttonGroup_->setExclusive(false);
	connect(buttonGroup_.data(), SIGNAL(buttonClicked(int)),this, SLOT(buttonGroupClicked(int)));
	connect(editorState.data(), SIGNAL(itemInserted()),this, SLOT(disableAllButtons()));
	layout_ = new QGridLayout;
	setLayout(layout_);
	clearButtons();
}

void ToolGroup::AddButton(const QString &label, QIcon icon, bool enabled)
{
    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
	button->setEnabled(enabled);
	int id=(2*row_)+col_;
    buttonGroup_->addButton(button,id);
	QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
	QLabel* widgLabel = new QLabel(label);
	widgLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(widgLabel, 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget(this);
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

QPixmap ToolGroup::getButtonPixmap(int buttonId)
{
	QAbstractButton* button = buttonGroup_->button(buttonId);
	if(!button)
		return QPixmap();
	return button->icon().pixmap(button->iconSize());
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
	if(id >= 0)
		doButtonAction(id);
	else
		disableButtonActions();
}

void ToolGroup::disableAllButtons()
{
	QList<QAbstractButton *> buttons = buttonGroup_->buttons();
	foreach (QAbstractButton *button, buttons) 
	{
		button->setChecked(false);
    }
	disableButtonActions();
}