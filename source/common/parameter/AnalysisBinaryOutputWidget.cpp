#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTimer>
#include "AnalysisBinaryOutputWidget.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

AnalysisBinaryOutputWidget::AnalysisBinaryOutputWidget(QWidget* parent)
: AnalysisOutputWidget(parent)
{
	textEdit_ = new QTextEdit();
	textEdit_->setText("Preview is not supported for binary files.");
	textEdit_->setReadOnly(true);
	QHBoxLayout* output = new QHBoxLayout();
	output->addWidget(textEdit_);
	QVBoxLayout* main = new QVBoxLayout();
	main->addLayout(output);
	setLayout(main);
}

AnalysisBinaryOutputWidget::~AnalysisBinaryOutputWidget()
{
}
/*! \brief Sets the file who's contents this widget will display (once the widget actually starts to display binary contents)
 *	\note The input filename should be the full name including filepath.
 */
bool AnalysisBinaryOutputWidget::setFile(QString filename)
{
	QString objName = filename.mid(filename.lastIndexOf("/")+1);
	if(objName.isEmpty())
		objName = filename;
	setObjectName(objName);
	file_ = QSharedPointer<QFile>(new QFile(filename));
	return true;
}

bool AnalysisBinaryOutputWidget::isSaveable()
{
	return true;
}
/*! \brief Copies the underlying temporary data file to the input directory, returns true if successful, false otherwise.
 *	\details If the copy operation fails, a message box pops up to that affect.
*/
bool AnalysisBinaryOutputWidget::saveOutputTo(QDir directory)
{	
	if(!file_)
		return false;
	if(!directory.exists())
		return false;	//Caller must verify that directory exists.
	QString directoryPath = directory.absolutePath();
	QString newFileName = directoryPath + "/" + objectName();
	QFile::remove(newFileName);
	bool result = file_->copy(newFileName);
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