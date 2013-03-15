#ifndef ASSETINFOBOX_H
#define ASSETINFOBOX_H
#include "EditorState.h"
#include "SearchableTextEdit.h"

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

//! [0]
class AssetInfoBox : public SearchableTextEdit
{
    Q_OBJECT

public:
   AssetInfoBox(QSharedPointer<EditorState> editorState,QWidget *parent=0);
   virtual ~AssetInfoBox(){};
public slots:
	void assetSelected(QSharedPointer<Asset> asset);

private:
	QSharedPointer<EditorState> editorState_;

private slots:
	void searchRequested(SearchRequest searchRequest);
};
//! [0]
#endif
