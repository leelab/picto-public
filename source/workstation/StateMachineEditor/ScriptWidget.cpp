#include <QtGui>
#include "ScriptWidget.h"

//! [0]
ScriptWidget::ScriptWidget(QtVariantPropertyManager* manager, QtProperty* property, QWidget *parent) :
	QTextEdit(parent),
	manager_(manager),
	property_(property)
{
	setLineWrapMode(NoWrap);
	setMinimumWidth(100);
	setTabStopWidth(16);
	QString text = manager_->value(property).toString();
	lineStartTabs_ = 0;
	QString finalStr = "";

	//Remove leading tabs and store the number of leading tabs so that they can be restored after editing.
	if(text.length())
	{
		QStringList lines = text.split("\n");
		int minTabs = -1;
		foreach(QString line,lines)
		{
			QString shortened = line.trimmed();
			if(!shortened.length())
				continue;
			QChar firstLetter = shortened.at(0);
			QStringList helper = line.split(firstLetter);
			if(helper.size() > 1)
			{
				int numTabs = helper.first().length();
				if((minTabs == -1) || (numTabs < minTabs))
				{
					minTabs = numTabs;
				}
			}
		}
		lineStartTabs_ = minTabs;

		//Now that we've got the number of leading tabs, take them out.
		foreach(QString line,lines)
		{
			if(!line.trimmed().length())
				finalStr.append(line);
			else
				finalStr.append(line.left(lineStartTabs_).remove("\t")+line.right(line.length()-lineStartTabs_));
			finalStr.append("\n");
		}
		finalStr.chop(1);
	}

	//Set fixed text value to widget.
	setText(finalStr);
	connect(this, SIGNAL(textChanged()),this, SLOT(setScriptValue()));
}

void ScriptWidget::setScriptValue()
{
	//Restore leading tabs.
	QString editedText = toPlainText();
	QStringList lines = editedText.split("\n");
	QString finalText = "";
	foreach(QString line, lines)
	{
		if(!line.trimmed().length())
			finalText.append(line);
		else
			finalText.append(QString("\t").repeated(lineStartTabs_)).append(line);
		finalText.append("\n");
	}
	finalText.chop(1);

	//Set fixed text value to property.
	manager_->setValue(property_,finalText);
}