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

class AnalysisFileOutputWidget : public AnalysisOutputWidget
{
	Q_OBJECT
public:
	AnalysisFileOutputWidget(unsigned int linesPerPage, QWidget* parent = 0);
	virtual ~AnalysisFileOutputWidget();

	bool setFile(QString filename);
	void refreshContents(){pages_.clear();currPage_ = -1;loadCurrPage();};
	virtual bool isSaveable();
	virtual bool saveOutputTo(QDir directory);
private:
	QSharedPointer<QFile> file_;
	QSharedPointer<QTextStream> outputFileStream_;
	QTextEdit* textEdit_;
	QPushButton* prev_;
	QPushButton* next_;
	struct PageDef
	{
		PageDef(){startPos = endPos = 0;};
		PageDef(int s,int e){startPos = s;endPos = e;};
		bool isValid(){return startPos < endPos;};
		int startPos;
		int endPos;
	};
	QVector<PageDef> pages_;
	int currPage_;
	int linesPerPage_;
private slots:
	void loadCurrPage();
	void prevPage();
	void nextPage();

};
}; //namespace Picto
#endif