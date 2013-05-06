#ifndef AboutElementBox_H
#define AboutElementBox_H
#include "EditorState.h"
#include "SearchableTextEdit.h"
#include "AssetDescriber.h"

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

//! [0]
class AboutElementBox : public SearchableTextEdit
{
    Q_OBJECT

public:
   AboutElementBox(QSharedPointer<EditorState> editorState,QWidget *parent=0);
   virtual ~AboutElementBox(){};
public slots:
	void assetSelected(QSharedPointer<Asset> asset);
	void elementOfInterestChanged(QString className, QString friendlyName);

private:
	QSharedPointer<EditorState> editorState_;
	QSharedPointer<AssetDescriber> assetDescriber_;

private slots:
	void searchRequested(SearchRequest searchRequest);
};
//! [0]
#endif
