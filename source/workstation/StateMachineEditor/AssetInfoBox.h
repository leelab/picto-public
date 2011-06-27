#ifndef ASSETINFOBOX_H
#define ASSETINFOBOX_H
#include "EditorState.h"
#include <QTextEdit>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

//! [0]
class AssetInfoBox : public QTextEdit
{
    Q_OBJECT

public:
   AssetInfoBox(QSharedPointer<EditorState> editorState,QWidget *parent=0);
public slots:
	void assetSelected(QSharedPointer<Asset> asset);

private:
	QSharedPointer<EditorState> editorState_;
};
//! [0]
#endif
