#include <QtWidgets>
#include "FileSelectWidget.h"
#include "../../common/memleakdetect.h"

/*! \brief Constructs a new FileSelectWidget. 
 *	\details manager and property are used to set/get the value of the underlying file path property.  editorState was originally used to include
 *	this property's contents in searches; however, it looks like we stopped supporting that, so we can probably remove it.
 */
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

	//IT LOOKS LIKE WE GOT RID OF searchRequested(), SO THIS NO LONGER DOES ANYTHING
	//Set up search
	connect(editorState_.data(),SIGNAL(searchRequested(SearchRequest)),this,SLOT(searchRequested(SearchRequest)));
}
/*! \brief Returns the file name part of the file path stored in this widget.
*/
QString FileSelectWidget::getFileName(QString filePath)
{
	int backslashLoc = filePath.lastIndexOf(QRegExp("[\\\\\\/]"));
	QString fileName = filePath.right(filePath.size()-(1+backslashLoc));
	return fileName;
}

/*! \brief Called when the designer presses the button to display a FileDialog, get the file selection and set it into this widget's referenced property.
 *	\details This function also resets the button label to the name part of the selected file path and emits the editingFinished() signal
 *	to let anyone who is interested know that file selection is complete.
 */
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
	//For this purpose, change the file path to an empty string and then back
	//to the current file path so that connected objects will detect a change
	//and reload the data
	if(filepath == lastFilePath)
		manager_->setValue(property_,"");
	manager_->setValue(property_,filepath);
	selectButton_->setText(getFileName(filepath));
	emit editingFinished();
}