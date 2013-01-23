#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include "TaskRunSelector.h"
#include "../common/globals.h"
#include "../../common/memleakdetect.h"
using namespace Picto;


TaskRunSelector::TaskRunSelector(QWidget *parent) :
	QWidget(parent)
{
	title_ = new QLabel("Select Runs for Analysis:");
	selectArea_ = new QListWidget();
	selectArea_->setSelectionMode(QAbstractItemView::MultiSelection);
	clear_ = new QPushButton("Clear");
	connect(clear_,SIGNAL(released()),this,SLOT(clearSelections()));
	selectAll_ = new QPushButton("SelectAll");
	connect(selectAll_,SIGNAL(released()),this,SLOT(selectAll()));
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(clear_);
	buttonLayout->addWidget(selectAll_);
	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(title_);
	mainLayout->addWidget(selectArea_);
	mainLayout->addLayout(buttonLayout);
	setLayout(mainLayout);
}

TaskRunSelector::~TaskRunSelector()
{
}

void TaskRunSelector::loadSessions(QList<QSqlDatabase> sessions)
{
	selectArea_->clear();
	runs_.clear();
	sessions_.clear();
	foreach(QSqlDatabase session,sessions)
	{
		QSqlQuery query(session);
		query.setForwardOnly(true);

		//Get frame value list.  If there is no last frame listed, the run might still be in progress or 
		//their was a failure somewhere and the session ended while the Run was still in progress.
		query.prepare("SELECT r.runid,r.name,r.notes,r.saved,r.firstframe,r.lastframe,s.time,e.time "
			"FROM runs r, frames s, frames e WHERE s.dataid=r.firstframe AND ((r.lastframe>0 AND e.dataid=r.lastframe) "
			"OR (r.lastframe=0 AND e.dataid=(SELECT dataid FROM frames ORDER BY dataid DESC LIMIT 1))) ORDER BY runid");
		bool success = query.exec();
		if(!success)
		{
			qDebug("Failed to select data from runs table with error: " + query.lastError().text().toLatin1());
			return;
		}
		while(query.next()){
			//continue if the run wasn't saved.
			if(!query.value(3).toBool())
				continue;
			//Build up the run object
			QSharedPointer<Picto::TaskRunDataUnit> run(
				new Picto::TaskRunDataUnit
					(
						query.value(4).toLongLong(),
						query.value(5).toLongLong(),
						query.value(1).toString(),
						query.value(2).toString(),
						query.value(3).toBool()
					)
				);
			run->setDataID(query.value(0).toLongLong());
			//Add RunData to the runs_ list.
			runs_.append(RunData(run,query.value(6).toDouble(),query.value(7).toDouble()));
			sessions_.append(session);
			//Add run name to the selection area.
			QListWidgetItem* runItem(new QListWidgetItem(run->name_));
			runItem->setData(32,selectArea_->count());
			runItem->setToolTip(run->notes_);
			selectArea_->addItem(runItem);	
		}
		query.finish();
	}
		if(!selectArea_->count())
		{
			QMessageBox box;
			box.setText("No Saved Runs                                      ");
			box.setDetailedText("No saved runs were found in these sessions.");
			box.setIconPixmap(QPixmap(":/icons/x.png"));
			box.exec();
		}
		else
			selectAll();
}

int TaskRunSelector::selectedRunCount()
{
	return selectArea_->selectedItems().count();
}

QSharedPointer<Picto::TaskRunDataUnit> TaskRunSelector::getSelectedRun(int index)
{
	return getRunDataFromSelectedIndex(index).run;
}

QSqlDatabase TaskRunSelector::getSessionForSelectedRun(int index)
{
	if(index >= sessions_.length())
	{
		Q_ASSERT(false);
		return QSqlDatabase();
	}
	return sessions_[index];
}

qulonglong TaskRunSelector::startFrameOfSelectedRun(int index)
{
	return getRunDataFromSelectedIndex(index).run->startFrame_;
}
qulonglong TaskRunSelector::endFrameOfSelectedRun(int index)
{
	return getRunDataFromSelectedIndex(index).run->endFrame_;
}
double TaskRunSelector::startTimeOfSelectedRun(int index)
{
	return getRunDataFromSelectedIndex(index).startTime;
}
double TaskRunSelector::endTimeOfSelectedRun(int index)
{
	return getRunDataFromSelectedIndex(index).endTime;
}
QString TaskRunSelector::nameOfSelectedRun(int index)
{
	return getRunDataFromSelectedIndex(index).run->name_;
}

TaskRunSelector::RunData TaskRunSelector::getRunDataFromSelectedIndex(int selIndex)
{
	return runs_[selectArea_->selectedItems()[selIndex]->data(32).toInt()];
}

void TaskRunSelector::clearSelections()
{
	selectArea_->clearSelection();
	selectArea_->setFocus();
}

void TaskRunSelector::selectAll()
{
	selectArea_->selectAll();
	selectArea_->setFocus();
}