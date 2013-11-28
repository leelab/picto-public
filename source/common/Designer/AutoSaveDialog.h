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
/*! \brief A dialog that presents the option to restore automatically saved design versions to the Picto designer when starting up after a crash.
 *	\details The widget presents all of the files in the list entered into the constructor.  "X"'s are presented next to each
 *	file and if clicked, a deleteFileRequest() is emitted and the dialog's contents updated.  There is also an "Open" button that
 *	allows for restoring a selected pre-saved file or "ignore" that closes the widget, maintaining all of the auto-saved files
 *	for next time the workstation is opened.
 *
 *	This widget is used with the AutoSaver to create a complete AutoSave system preventing significant data loss should Picto
 *	or the machine running it crash unexpectedly.
 *	\sa AutoSaver
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
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

#endif
