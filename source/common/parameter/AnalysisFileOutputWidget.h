#ifndef _FILE_OUTPUT_WIDGET_H_
#define _FILE_OUTPUT_WIDGET_H_

#include <QSharedPointer>
#include <QFile>
#include <QTextStream>
#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVector>
#include <QLabel>
#include "AnalysisOutputWidget.h"

namespace Picto {

/*!	\brief An AnalysisOutputWidget for displaying textual file contents.
 *	\details This widget contains a read-only text box, a forward button and a backward button.
 *	Page lengths are defined as the number of lines input in the constructor as linesPerPage.
 *	Pressing the forward and backward buttons navigates forward and backward from page to page.
 *	Currently, this widget is used exclusively with the AnalysisFileOutput object to display the
 *	contents of its underlying output file.
 *	
 *	We use the paging system instead of creating one giant scrollable text box because typical Picto 
 *	Analyses can contain many megabytes of data and adding a multimegabyte text edit widget
 *	to the UI slows things down drastically and is a memory hog.  By creating a "paged" view we
 *	are able to easily limit the amount of data that we need to store in RAM and display on screen.  
 *	With that said, if someone decides to throw tons of text into the AnalysisFileOutput without line breaks, we
 *	will run into problems since our page is defined in number of lines, not word count.  We
 *	may want to look more closely at this at some point.
 *	\sa AnalysisFileOutput
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class AnalysisFileOutputWidget : public AnalysisOutputWidget
{
	Q_OBJECT
public:
	AnalysisFileOutputWidget(unsigned int linesPerPage, QWidget* parent = 0);
	virtual ~AnalysisFileOutputWidget();

	bool setFile(QString filename);
	/*! \brief Refreshes the contents of this widget from scratch.*/
	void refreshContents(){pages_.clear();currPage_ = -1;loadCurrPage();};
	virtual bool isSaveable();
	virtual bool saveOutputTo(QDir directory);
private:
	void patchQtTextStreamBug();
	QSharedPointer<QFile> file_;
	QSharedPointer<QTextStream> outputFileStream_;
	QTextEdit* textEdit_;
	QPushButton* prev_;
	QPushButton* next_;
	struct PageDef
	{
		PageDef(){startPos = endPos = completePage = 0;};
		PageDef(int s,int e,bool c){startPos = s;endPos = e;completePage = c;};
		bool isValid(){return startPos < endPos;};
		int startPos;
		int endPos;
		bool completePage;
	};
	QVector<PageDef> pages_;
	int currPage_;
	int lastPage_;
	int linesPerPage_;
private slots:
	void loadCurrPage();
	void prevPage();
	void nextPage();

};
}; //namespace Picto
#endif