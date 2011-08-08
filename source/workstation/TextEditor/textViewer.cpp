#include "textviewer.h"

#include <QVBoxLayout>
#include <QTextEdit>
#include <QComboBox>
#include <QToolBar>
#include <QLabel>
#include <QDebug>
#include <QAction>

#include <QXmlStreamReader>

TextViewer::TextViewer(QAction* checkSyntaxAction, QWidget *parent)
: Viewer(parent)
{
	xmlEdit_ = new XmlEditor();
	syntaxHighlight_ = new SyntaxHighlighter(xmlEdit_);
	connect(xmlEdit_, SIGNAL(cursorPositionChanged()), this, SLOT(updateStatus()));

	taskComboBox_ = new QComboBox(this);
	taskComboBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	fillTaskComboBox();

	editToolBar_ = new QToolBar(this);
	editToolBar_->addWidget(new QLabel("Task:",this));
	editToolBar_->addWidget(taskComboBox_);
	editToolBar_->addSeparator();
	editToolBar_->addAction(checkSyntaxAction);

	statusLabel_ = new QLabel("Line: 9999, Column: 999");

	mainLayout_ = new QVBoxLayout;
	mainLayout_->addWidget(editToolBar_);
	mainLayout_->addWidget(xmlEdit_);
	mainLayout_->addWidget(statusLabel_);
	setLayout(mainLayout_);

	
	init();
}

//! Called just before displaying the text viewer
void TextViewer::init()
{
	//Put the current experiment text into the text editor
	if(pictoDataText_)
	{
		xmlEdit_->setDocument(pictoDataText_);
		syntaxHighlight_->setDocument(pictoDataText_);

		//set tabs equal to four spaces
		QFont font("Courier",10);
		QFontMetrics metrics(font);
		xmlEdit_->setTabStopWidth(metrics.width("    "));

		xmlEdit_->document()->setDefaultFont(font);

		fillTaskComboBox();
	}
}

//!Called just before hiding the text viewer
void TextViewer::deinit()
{
	//The main window handles converting the text to an object,
	//so we don't really need to do anything here.
}
/*!	\brief Fills the task combo box with the names of the tasks contained in the experiment
 *
 *	This is kind of a tricky function.  I'm not sure that this is the most elegant
 *	solution, nor that it will work 100% of the time.  This code assumes that
 *	the first <Name> element encountered is a direct child of the <Task> element.
 *	Since we aren't requiring a specific ordering of tags, this may or may not 
 *	be the case.  We should probably go back and make name an attribute of Task
 *	but if we do that, we have to do it for a bunch of other objects as well...
 */
void TextViewer::fillTaskComboBox()
{
	disconnect(taskComboBox_, SIGNAL(currentIndexChanged(QString)), this, SLOT(goToTask(QString)));
	
	taskComboBox_->clear();
	taskComboBox_->addItem("Experiment top");

	if(!pictoDataText_)
		return;

	//Parse the experiment text looking for all tasks, extract their names, and 
	//add the name to the combo box
	QXmlStreamReader xmlReader(pictoDataText_->toPlainText());

	while(!xmlReader.atEnd())
	{
		if(xmlReader.name() == "Task" && xmlReader.isStartElement())
		{
			while(xmlReader.name() != "Name" && !xmlReader.atEnd())
				xmlReader.readNext();
			if(!xmlReader.atEnd())
			{
				QString taskName = xmlReader.readElementText();
				taskComboBox_->addItem(taskName);
			}
		}
		xmlReader.readNext();
	}
	connect(taskComboBox_, SIGNAL(currentIndexChanged(QString)), this, SLOT(goToTask(QString)));
}

/*!	\brief Moves the cursor to the <Task> tag for the task with the matching name
 *
 *	This makes the same assumptions as the fillTaskComboBox function.
 */
void TextViewer::goToTask(QString taskname)
{
	xmlEdit_->setFocus();
	if(taskname == "Experiment top")
	{
		xmlEdit_->moveCursor(QTextCursor::Start);
		xmlEdit_->moveCursor(QTextCursor::StartOfLine);
		xmlEdit_->moveCursor(QTextCursor::EndOfLine,QTextCursor::KeepAnchor);
		return;
	}

	//Find the line number of the task with the matching name
	QXmlStreamReader xmlReader(pictoDataText_->toPlainText());

	int taskOffset;

	while(!xmlReader.atEnd())
	{
		if(xmlReader.name() == "Task" && xmlReader.isStartElement())
		{
			taskOffset = xmlReader.characterOffset();

			while(xmlReader.name() != "Name" && !xmlReader.atEnd())
				xmlReader.readNext();
			if(!xmlReader.atEnd())
			{
				if(taskname == xmlReader.readElementText())
				{
					//We do some fancy cursor moving in an attempt to make the
					//selected task obvious
					xmlEdit_->moveCursor(QTextCursor::End);
					QTextCursor cursor = xmlEdit_->textCursor();
					cursor.setPosition(taskOffset);
					cursor = xmlEdit_->document()->find("<Task>",cursor, QTextDocument::FindBackward);
					xmlEdit_->setTextCursor(cursor);
					break;
				}
			}
		}
		xmlReader.readNext();
	}
}

//! \brief This slot updates the status label with the current line/column
void TextViewer::updateStatus()
{
	int line, col;

	//NOTE: The block number is only the line number because this is a simple document.
	//In a more complex text editor block number is not the same as line number.
	line = xmlEdit_->textCursor().blockNumber() + 1;
	col = xmlEdit_->textCursor().columnNumber();

	QString lineStr = QString::number(line);
	QString colStr = QString::number(col);
	statusLabel_->setText("Line: " + lineStr +" Col: "+colStr);
}