#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QSqlError>
#include "AnalysisOutputDisplay.h"
#include "../common/globals.h"
#include "../../common/memleakdetect.h"
using namespace Picto;


AnalysisOutputDisplay::AnalysisOutputDisplay(QWidget *parent) :
	QWidget(parent)
{
	outputBox_ = NULL;
	mainLayout_ = new QVBoxLayout;
	setLayout(mainLayout_);
	clear();
}

AnalysisOutputDisplay::~AnalysisOutputDisplay()
{
}

void AnalysisOutputDisplay::addOutputTab(QString name,QWidget* widget)
{
	outputBox_->addTab(widget,name);
}

void AnalysisOutputDisplay::clear()
{
	if(outputBox_)
	{
		mainLayout_->removeWidget(outputBox_);
		outputBox_->setParent(NULL);
		delete outputBox_;
	}
	outputBox_ = new QTabWidget();
	mainLayout_->addWidget(outputBox_);
}