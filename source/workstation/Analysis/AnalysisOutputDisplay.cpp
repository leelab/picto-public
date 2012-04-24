#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QSqlError>
#include "../../common/analysis/AnalysisOutputWidget.h"
#include "AnalysisOutputDisplay.h"
#include "../common/globals.h"
#include "../../common/memleakdetect.h"
using namespace Picto;


AnalysisOutputDisplay::AnalysisOutputDisplay(QWidget *parent) :
	QWidget(parent)
{
	topLevelTabs_ = NULL;
	mainLayout_ = new QVBoxLayout;
	setLayout(mainLayout_);
	clear();
}

AnalysisOutputDisplay::~AnalysisOutputDisplay()
{
}

int AnalysisOutputDisplay::addTopLevelTab(QString name)
{
	QTabWidget* subTabs(new QTabWidget());
	topLevelTabs_->addTab(subTabs,name);
	return topLevelTabs_->count()-1;
}

void AnalysisOutputDisplay::addSubTab(int topLevelIndex,QString name,QWidget* widget)
{
	Q_ASSERT(topLevelIndex < topLevelTabs_->count());
	QTabWidget* subTabs = qobject_cast<QTabWidget*>(topLevelTabs_->widget(topLevelIndex));
	subTabs->addTab(widget,name);
}

bool AnalysisOutputDisplay::supportsSaving()
{
	for(int i=0;i<topLevelTabs_->count();i++)
	{
		QTabWidget* subTabs = qobject_cast<QTabWidget*>(topLevelTabs_->widget(i));
		for(int j=0;j<subTabs->count();j++)
		{
			Picto::AnalysisOutputWidget *subWidget = qobject_cast<Picto::AnalysisOutputWidget *>(subTabs->widget(j));
			if(subWidget->isSaveable())
				return true;
		}
	}
	return false;
}

bool AnalysisOutputDisplay::saveOutputToDirectory(QDir directory)
{
	if(!directory.exists())
		return false;	//Caller must verify that directory exists.
	for(int i=0;i<topLevelTabs_->count();i++)
	{
		QString subDirName = topLevelTabs_->tabText(i);
		directory.mkdir(subDirName);
		QDir subDir = directory;
		if(!subDir.cd(subDirName))
		{
			QMessageBox error;
			error.setText("Could not create sub-directory");
			error.setDetailedText("Could not create: " + directory.absolutePath() + "/" + subDirName);
			error.setIcon(QMessageBox::Critical);
			error.exec();
			return false;
		}
		QTabWidget* subTabs = qobject_cast<QTabWidget*>(topLevelTabs_->widget(i));
		for(int j=0;j<subTabs->count();j++)
		{
			Picto::AnalysisOutputWidget *subWidget = qobject_cast<Picto::AnalysisOutputWidget *>(subTabs->widget(j));
			if(subWidget->isSaveable())
				if(!subWidget->saveOutputTo(subDir))
					return false;
		}
	}
	return true;
}

void AnalysisOutputDisplay::clear()
{
	if(topLevelTabs_)
	{
		mainLayout_->removeWidget(topLevelTabs_);
		topLevelTabs_->setParent(NULL);
		delete topLevelTabs_;
	}
	topLevelTabs_ = new QTabWidget();
	mainLayout_->addWidget(topLevelTabs_);
}