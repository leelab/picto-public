#include <QtWidgets>
#include "FileSelectWidget.h"
#include "../../common/memleakdetect.h"

//! [0]
FileSelectWidget::FileSelectWidget(QtVariantPropertyManager* manager, QtProperty* property, QSharedPointer<EditorState> editorState, QWidget *parent) :
	QWidget(parent),
	manager_(manager),
	property_(property),
	selectButton_(new QToolButton()),
	layout_(new QVBoxLayout()),
	editorState_(editorState)
{
	origButtonColor_ = selectButton_->palette().color(QPalette::Button);
	connect(selectButton_,SIGNAL(released()),this,SLOT(selectFile()));
	layout_->addWidget(selectButton_);
	layout_->setContentsMargins(QMargins(0,0,0,0));
	setLayout(layout_);
	QString filepath = manager_->value(property_).toString();
	if(filepath.isEmpty())
		selectButton_->setText("Select file...");
	else
		selectButton_->setText(getFileName(filepath));

	//Set up search
	connect(editorState_.data(),SIGNAL(searchRequested(SearchRequest)),this,SLOT(searchRequested(SearchRequest)));
}

QString FileSelectWidget::getFileName(QString filePath)
{
	int backslashLoc = filePath.lastIndexOf(QRegExp("[\\\\\\/]"));
	QString fileName = filePath.right(filePath.size()-(1+backslashLoc));
	return fileName;
}

void FileSelectWidget::selectFile()
{
	QString lastFilePath = manager_->value(property_).toString();
	if(lastFilePath.isEmpty())
		lastFilePath = ".";
	//else
	//	lastFilePath = lastFilePath.left(lastFilePath.lastIndexOf(QRegExp("[\\\\\\/]")));
	QString filepath = QFileDialog::getOpenFileName(this,
		tr("Select File"),lastFilePath,"*.*");
	if(filepath.isEmpty())
		return;
	//If the file path hasn't changed, we want to reload the file.
	//For this purpose, change the file path to an empty string and then bac'
	//to the current file path so that connected objects will detect a change
	//and reload the data
	if(filepath == lastFilePath)
		manager_->setValue(property_,"");
	manager_->setValue(property_,filepath);
	selectButton_->setText(getFileName(filepath));
	emit editingFinished();
}

//void FileSelectWidget::searchRequested(SearchRequest searchRequest)
//{
//	if(searchRequest.type != SearchRequest::STRING)
//		return;
//	QPalette p = selectButton_->palette();
//	if(selectButton_->text().contains(searchRequest.query,searchRequest.caseSensitive?Qt::CaseSensitive:Qt::CaseInsensitive))
//	{
//		p.setColor(QPalette::Button, QColor(255,0,0,100));
//	}
//	else
//	{
//		p.setColor(QPalette::Button, origButtonColor_);
//	}
//	selectButton_->setPalette(p);
//}