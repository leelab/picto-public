#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTimer>
#include "AnalysisFileOutputWidget.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

/*! \brief Creates a new AnalysisFileOutputWidget.
 *	\details Sets up the widget layout including the main read only text edit and the
 *	forward/back buttons.
 *	linesPerPage is the number of lines of text that should be displayed per page within the
 *	widget.
 */
AnalysisFileOutputWidget::AnalysisFileOutputWidget(unsigned int linesPerPage, QWidget* parent)
: AnalysisOutputWidget(parent)
{
	linesPerPage_ = linesPerPage;
	textEdit_ = new QTextEdit();
	textEdit_->setReadOnly(true);
	prev_ = new QPushButton(QIcon(":/icons/prev.png"),"Previous");
	connect(prev_,SIGNAL(released()),this,SLOT(prevPage()));
	next_ = new QPushButton(QIcon(":/icons/next.png"),"Next");
	connect(next_,SIGNAL(released()),this,SLOT(nextPage()));
	
	QHBoxLayout* tools = new QHBoxLayout();
	tools->addWidget(prev_);
	tools->addWidget(next_);
	QHBoxLayout* output = new QHBoxLayout();
	output->addWidget(textEdit_);
	QVBoxLayout* main = new QVBoxLayout();
	main->addLayout(output);
	main->addLayout(tools);
	main->setStretch(0,10);
	main->setStretch(1,1);
	setLayout(main);
}

AnalysisFileOutputWidget::~AnalysisFileOutputWidget()
{
	if(file_)
		file_->close();
}

/*! \brief Sets the file who's contents this widget will display.
 *	\details This function also opens the file and starts a timer that
 *	calls loadCurrPage() to refresh the contents of the file once every 3 seconds so
 *	that the widget will be updated with new data as it comes in during an actively running
 *	Analysis.
 *	\note The input filename should be the full name including filepath.
 */
bool AnalysisFileOutputWidget::setFile(QString filename)
{
	pages_.clear();
	textEdit_->clear();
	currPage_ = -1;
	lastPage_ = -1;
	QString objName = filename.mid(filename.lastIndexOf("/")+1);
	if(objName.isEmpty())
		objName = filename;
	setObjectName(objName);
	if(file_)
		file_->close();
	file_ = QSharedPointer<QFile>(new QFile(filename));
	if(!file_->open(QIODevice::ReadOnly | QIODevice::Text))
		return false;
	//Set up a timer to periodically check file contents and re-read them
	QTimer* readTimer(new QTimer(this));
	connect(readTimer,SIGNAL(timeout()),this,SLOT(loadCurrPage()));
	readTimer->setInterval(3000);
	readTimer->start();

	outputFileStream_ = QSharedPointer<QTextStream>(new QTextStream(file_.data()));
	prev_->setEnabled(false);
	loadCurrPage();
	return true;
}

/*! \brief Moves the currently displayed page backwards to the previous page.
 *	\details Pages are defined by numbers of lines (linesPerPage input into the constructor).
 *	\note Things can slow down pretty drastically in the GUI if there are no line breaks in the
 *	file.  We should probably do something about this.
 */
void AnalysisFileOutputWidget::prevPage()
{
	if(currPage_ == 0)
		return;
	currPage_--;
	loadCurrPage();
	textEdit_->setCursor(QCursor());
}

/*! \brief Moves the currently displayed page forwards to the next page.
 *	\details Pages are defined by numbers of lines (linesPerPage input into the constructor).
 *	\note Things can slow down pretty drastically in the GUI if there are no line breaks in the
 *	file.  We should probably do something about this.
 */

void AnalysisFileOutputWidget::nextPage()
{
	currPage_++;
	loadCurrPage();
	textEdit_->setCursor(QCursor());
}

bool AnalysisFileOutputWidget::isSaveable()
{
	return true;
}
/*! \brief Copies the underlying temporary data file to the input directory, returns true if successful, false otherwise.
 *	\details If the copy operation fails, a message box pops up to that affect.
*/
bool AnalysisFileOutputWidget::saveOutputTo(QDir directory)
{	
	if(!file_)
		return false;
	if(!directory.exists())
		return false;	//Caller must verify that directory exists.
	QString directoryPath = directory.absolutePath();
	QString newFileName = directoryPath + "/" + objectName();
	QFile::remove(newFileName);
	//Using file_->copy() causes the file to be closed, so we use the static function
	bool result = QFile::copy(file_->fileName(),newFileName);
	if(!result)
	{
		QMessageBox error;
		error.setText("Could not create save file");
		error.setDetailedText("Could not create: " + newFileName);
		error.setIcon(QMessageBox::Critical);
		error.exec();
		return false;
	}
	return result;
}

/*! \brief Patches a QTextStream bug.
 *	\details There is some kind of Qt bug that makes the text stream stop functioning intermittently, possibly due to 
 *	the text pointer reaching the end of the file stream.  In these cases, pos() returns -1.  To fix the issue
 *	whenever this occurs, we recreate the QTextStream from scratch.
 */
void AnalysisFileOutputWidget::patchQtTextStreamBug()
{
	if(outputFileStream_->pos() < 0)
		outputFileStream_ = QSharedPointer<QTextStream>(new QTextStream(file_.data()));
}

/*! \brief Loads the current page of text into the widget's read-only text edit.
 *	\details If new data is available in the underlying file, it will show up in the text edit.
 *	If the forward or back buttons were pressed, the widget will show the next or previous page
 *	of data if it exists.
 */
void AnalysisFileOutputWidget::loadCurrPage()
{
	//Disable previous and next buttons
	prev_->setEnabled(false);
	next_->setEnabled(false);

	if(currPage_ < 0)
		currPage_ = 0;

	int lastPageEnd = 0;
	int newPageEnd = -1;
	
	//If we don't yet have all the page limits defined up the current page selected by the user
	while(currPage_ >= pages_.size() && !outputFileStream_->atEnd())
	{	//Expand pages_ definitions until we reach the desired page
		bool finishingOldPage = false;
		if(pages_.size())
		{
			if(pages_.last().completePage)
			{
				lastPageEnd = pages_.last().endPos;
			}
			else
			{
				finishingOldPage = true;
				lastPageEnd = pages_.last().startPos;
			}
		}
		patchQtTextStreamBug();
		if(!outputFileStream_->seek(lastPageEnd)
			|| outputFileStream_->atEnd())
			break;
		bool pageComplete = true;
		for(int i=0;i<linesPerPage_;i++)
		{
			outputFileStream_->readLine();
			if(outputFileStream_->atEnd())
			{
				break;
			}	
		}
		int currPos = outputFileStream_->pos();	//Note: putting pos() inside the above loop makes things really slow!!
		if(currPos < 0 || outputFileStream_->atEnd())
			pageComplete = false;
		newPageEnd = currPos;
		if(finishingOldPage)
			pages_[pages_.size()-1] = PageDef(lastPageEnd,newPageEnd,pageComplete);
		else
			pages_.append(PageDef(lastPageEnd,newPageEnd,pageComplete));
	}
	//If pages_ still isn't big enough, we must have hit the end of the file.  Correct currPage_
	if(currPage_ >= pages_.size())
	{	//We must have run out of data.  Show the last page with data.
		currPage_ = pages_.size() - 1;
	}

	patchQtTextStreamBug();
	//If there is no data in the file or we cant seek the beginning of the current page.  
	//Clear output and go back to the beginning of the file.
	if(pages_.empty() || !outputFileStream_->seek(pages_[currPage_].startPos))
	{
		//Something went wrong.  Start from the beginning.
		lastPage_ = -1;
		currPage_ = -1;
		textEdit_->setText("");
		return;
	}

	//I would have liked to use the QTextStream::read() function to read the text for this page in all in
	//one shot.  The problem was that for some reason, when specifying the number of characters to read in
	//with the read function, you have to skip "\n" characters.  This means that if we simply used the 
	//pages_[currPage_].endPos-pages_[currPage_].startPos, the value would be too high.  We could simply
	//delete the number of "\n" characters from the read() input, but that would mean recording the number
	//of lines read on the last page.  It also isn't clear if there are other characters that aren't
	//counted by read() as well (whitespace for example).  By simply reading in linesPerPage_ lines, we 
	//are assured that we will come out with our expected result.
	QString newLine;
	QString outputText;
	bool hasNextPage = true;
	for(int i=0;i<linesPerPage_;i++)
	{
		newLine = outputFileStream_->readLine();
		outputText.append(newLine).append("\n");
		if(outputFileStream_->atEnd())
		{
			hasNextPage = false;
			break;
		}
	}
	if(!outputText.isEmpty())
		outputText.resize(outputText.size()-1);	//Remove final "\n"

	if(textEdit_->toPlainText() != outputText)
	{
		bool isNewPage = (currPage_  != lastPage_);
		QTextCursor cursor(textEdit_->textCursor());
		textEdit_->setPlainText(outputText);
		if(!isNewPage)
			textEdit_->setTextCursor(cursor);
	}
	lastPage_ = currPage_;

	//Enable appropriate prev,next buttons
	if(currPage_ <= 0)
		prev_->setEnabled(false);
	else
		prev_->setEnabled(true);

	if(currPage_ < pages_.size()-1 || hasNextPage)
		next_->setEnabled(true);
	else
	{
		//patchQtTextStreamBug();
		////If there's more data, we just didn't create the pages for it, enable next, otherwise disable it
		//int nextAvailablePosition = pages_.empty()?0:pages_.last().endPos;
		//if(outputFileStream_->seek(nextAvailablePosition) && !outputFileStream_->atEnd())
		//	next_->setEnabled(true);
		//else
		next_->setEnabled(false);
	}
}