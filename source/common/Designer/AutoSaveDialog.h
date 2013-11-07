#ifndef AutoSaveDialog_H
#define AutoSaveDialog_H

#include <QWidget>
#include <QSharedPointer>
#include <QTabWidget>
#include <QGroupBox>
#include <QAbstractButton>
#include <QHash>
#include <QScrollArea>
#include <QDialog>
#include <QHash>
#include <QLabel>
#include "../design/DesignRoot.h"
#include "../parameter/Analysis.h"

namespace Picto {
//! [0]
//Widget for selecting from a lit of available Analyses
#if defined WIN32 || defined WINCE
class PICTOLIB_API AutoSaveDialog : public QDialog
#else
class AutoSaveDialog : public QDialog
#endif
{
    Q_OBJECT

public:
	AutoSaveDialog(QStringList files,QWidget *parent);
	virtual ~AutoSaveDialog(){};
signals:
	void deleteFileRequest(QString filePath);
	void restoreFileRequest(QString filePath);
private:
	QLabel* instructions_;
	QWidget* mainWidget_;
	QScrollArea* scrollArea_;
	QPushButton* restoreButton_;
	QPushButton* ignoreButton_;
	QSharedPointer<QButtonGroup> buttonGroup_;
	QSharedPointer<QButtonGroup> deleteButtonGroup_;
	QStringList files_;
	QHash<int,QString> filePathLookup_;
	QIcon deleteIcon_;

private slots:
	void updateContents();
	void checkboxChanged(bool checked);
	void deleteTriggered(int buttonIndex);
	void restoreTriggered(bool);
	void ignoreTriggered(bool);
};
};
//! [0]
#endif
