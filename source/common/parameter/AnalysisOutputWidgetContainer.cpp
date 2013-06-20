#include <QVBoxLayout>
#include <QMessageBox>
#include "AnalysisOutputWidgetContainer.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

AnalysisOutputWidgetContainer::AnalysisOutputWidgetContainer(QString runName, QWidget* parent)
: QWidget(parent)
{
	runName_ = runName;
	fractionOutputSaved_ = 0;
	QVBoxLayout* mainLayout = new QVBoxLayout();
	tabWidget_ = new QToolBox();
	tabWidget_->setVisible(false);
	mainLayout->addWidget(tabWidget_);
	setLayout(mainLayout);
}

AnalysisOutputWidgetContainer::~AnalysisOutputWidgetContainer()
{
}

bool AnalysisOutputWidgetContainer::isSaveable()
{
	foreach(QUuid widgetId,widgetLookup_.keys())
	{
		if(!enabledLookup_.contains(widgetId) || !enabledLookup_.value(widgetId))
			continue;
		Q_ASSERT(widgetLookup_.contains(widgetId));
		Picto::AnalysisOutputWidget *subWidget = widgetLookup_.value(widgetId);
		if(subWidget->isSaveable())
			return true;
	}
	return false;
}

bool AnalysisOutputWidgetContainer::saveOutputTo(QDir directory, bool separateRunDirs)
{
	if(!isSaveable())
		return false;
	fractionOutputSaved_ = 0;
	if(!directory.exists())
		return false;	//Caller must verify that directory exists.
	foreach(QUuid widgetId,widgetLookup_.keys())
	{
		QString subDirName = runName_;
		QDir subDir = directory;
		if(separateRunDirs)
		{
			directory.mkdir(subDirName);
			if(!subDir.cd(subDirName))
			{
				QMessageBox error;
				error.setText("Could not create sub-directory");
				error.setDetailedText("Could not create: " + directory.absolutePath() + "/" + subDirName);
				error.setIcon(QMessageBox::Critical);
				error.exec();
				return false;
			}
		}

		Picto::AnalysisOutputWidget *subWidget = widgetLookup_.value(widgetId);
		if(enabledLookup_.value(widgetId) && subWidget->isSaveable())
			if(!subWidget->saveOutputTo(subDir))
				return false;
		fractionOutputSaved_ += 1.0/tabWidget_->count();
	}
	return true;
}

void AnalysisOutputWidgetContainer::addWidget(QUuid identifier, AnalysisOutputWidget* widget)
{
	widgetLookup_[identifier] = widget;
	enabledLookup_[identifier] = false;
	QString widgetName = widget->getName();
	tabWidget_->addItem(widget,widgetName);
	tabWidget_->setItemToolTip(tabWidget_->count()-1,widgetName);
	tabWidget_->setItemEnabled(tabWidget_->count()-1,false);
	widget->setVisible(false);
}

void AnalysisOutputWidgetContainer::widgetIsValid(QUuid widgetIdentifier)
{
	if(!widgetLookup_.contains(widgetIdentifier))
		return;
	widgetLookup_.value(widgetIdentifier)->setVisible(true);
	enabledLookup_[widgetIdentifier] = true;
	int widgetIndex = tabWidget_->indexOf(widgetLookup_.value(widgetIdentifier));
	Q_ASSERT(widgetIndex >= 0 && widgetIndex < tabWidget_->count());
	tabWidget_->setItemEnabled(widgetIndex,true);
	tabWidget_->setVisible(true);
	if(isSaveable())
		emit detectedSaveable();
}