#ifndef FileSelectWidget_H
#define FileSelectWidget_H

#include <QWidget>
#include <QToolButton>
#include <QColor>
#include <QtVariantProperty.h>
#include "EditorState.h"

/*! \brief A file select button used to choose a file from disk.
 *	\details This widget is used in the PropertyBrowser for design elements that require a file path to be selected.
 *	The widget is a simple button that when pressed opens a file dialog.  Before a file has been selected, the button label
 *	is "Select File", after a file has been selected, the name of the file is used for the label.  The Property that this
 *	widget interacts with stores the file path.  When we use this in Picto, the Design Element typically checks to see when
 *	the File Path Property changes, and when it does, it updates the contents of separate Data Property that actually
 *	stores the file contents.
 *	\sa FileParameter
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class FileSelectWidget : public QWidget
{
    Q_OBJECT

public:
   FileSelectWidget(QtVariantPropertyManager* manager, QtProperty* property, QSharedPointer<EditorState> editorState, QWidget *parent=0);
   virtual ~FileSelectWidget(){};
signals:
   /*! \brief Emitted when the widget's button has been pressed and a file selection was completed.*/
   void editingFinished();
private:
	QString getFileName(QString filePath);
	QToolButton* selectButton_;
	QLayout* layout_;
	QtVariantPropertyManager *manager_;
	QtProperty *property_;
	QSharedPointer<EditorState> editorState_;
	QColor origButtonColor_;
private slots:
	void selectFile();
};


#endif
