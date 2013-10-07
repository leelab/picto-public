#ifndef FileSelectWidget_H
#define FileSelectWidget_H

#include <QWidget>
#include <QToolButton>
#include <QColor>
#include <QtVariantProperty.h>
#include "EditorState.h"

//! [0]
//Widget for presenting a file select option in the experimental designer property window.  Appears as a
//button.  Button name if no file has been selected in "Select File", otherwise it is the name of the 
//selected file.
class FileSelectWidget : public QWidget
{
    Q_OBJECT

public:
   FileSelectWidget(QtVariantPropertyManager* manager, QtProperty* property, QSharedPointer<EditorState> editorState, QWidget *parent=0);
   virtual ~FileSelectWidget(){};
signals:
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

//! [0]
#endif
