#ifndef _BINARY_OUTPUT_WIDGET_H_
#define _BINARY_OUTPUT_WIDGET_H_

#include <QSharedPointer>
#include <QFile>
#include <QWidget>
#include <QTextEdit>
#include <QLabel>
#include "AnalysisOutputWidget.h"

namespace Picto {

class AnalysisBinaryOutputWidget : public AnalysisOutputWidget
{
	Q_OBJECT
public:
	AnalysisBinaryOutputWidget(QWidget* parent = 0);
	virtual ~AnalysisBinaryOutputWidget();

	bool setFile(QString filename);
	virtual bool isSaveable();
	virtual bool saveOutputTo(QDir directory);
private:
	QSharedPointer<QFile> file_;
	QTextEdit* textEdit_;
};
}; //namespace Picto
#endif