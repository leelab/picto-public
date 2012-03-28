#include <QVBoxLayout>
#include <QHBoxLayout>
#include "FileOutputWidget.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

FileOutputWidget::FileOutputWidget(unsigned int linesPerPage, QWidget* parent)
: QWidget(parent)
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

FileOutputWidget::~FileOutputWidget()
{
}

bool FileOutputWidget::setFile(QString filename)
{
	pages_.clear();
	textEdit_->clear();
	currPage_ = -1;
	QString objName = filename.mid(filename.lastIndexOf("/")+1);
	if(objName.isEmpty())
		objName = filename;
	setObjectName(objName);
	file_ = QSharedPointer<QFile>(new QFile(filename));
	if(!file_->open(QIODevice::ReadOnly | QIODevice::Text))
		return false;
	outputFileStream_ = QSharedPointer<QTextStream>(new QTextStream(file_.data()));
	prev_->setEnabled(false);
	loadCurrPage();
	return true;
}

void FileOutputWidget::prevPage()
{
	currPage_--;
	loadCurrPage();
}

void FileOutputWidget::nextPage()
{
	currPage_++;
	loadCurrPage();
}

void FileOutputWidget::loadCurrPage()
{
	if(currPage_ < 0)
		currPage_ = 0;

	int lastPageEnd = 0;
	int newPageEnd = -1;
	while(currPage_ >= pages_.size() && !outputFileStream_->atEnd())
	{	//Expand pages_ definitions until we reach the desired page
		if(pages_.size())
			lastPageEnd = pages_.last().endPos;
		if(!outputFileStream_->seek(lastPageEnd)
			|| outputFileStream_->atEnd())
			break;
		for(int i=0;i<linesPerPage_;i++)
		{
			if(outputFileStream_->readLine().isNull())
				break;
		}
		newPageEnd = outputFileStream_->pos();
		if(newPageEnd < 0)
			return;	//This happens if an error occurs.
		if(newPageEnd < lastPageEnd)
			break;	//No data was on this page.
		pages_.append(PageDef(lastPageEnd,newPageEnd));
	}
	//If pages_ still isn't big enough, we must have hit the end of the file.  Correct currPage_
	if(currPage_ >= pages_.size())
	{	//We must have run out of data.  Show the last page with data.
		currPage_ = pages_.size() - 1;
	}

	//Enable appropriate prev,next buttons
	if(currPage_ == 0)
		prev_->setEnabled(false);
	else
		prev_->setEnabled(true);
	if(	pages_.empty() 
		|| !outputFileStream_->seek(pages_[currPage_].endPos) 
		|| outputFileStream_->atEnd())
		next_->setEnabled(false);
	else
		next_->setEnabled(true);

	//If there is no data in the file, pages_ will be empty.  
	//In this case clear output and we're done.
	if(pages_.empty())
	{
		textEdit_->setText("");
		return;
	}

	//Read page data into textEdit_.  Start by seeking the startPosition for this page.
	if(!outputFileStream_->seek(pages_[currPage_].startPos))
		return;
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
	for(int i=0;i<linesPerPage_;i++)
	{
		newLine = outputFileStream_->readLine();
		if(newLine.isEmpty())
			break;
		outputText.append(newLine).append("\n");
	}
	if(!outputText.isEmpty())
		outputText.resize(outputText.size()-1);	//Remove final "\n"

	textEdit_->setText(outputText);
}