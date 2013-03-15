#ifndef LEVELUPBUTTON_H
#define LEVELUPBUTTON_H

#include <QToolButton>
#include <QSharedPointer>
#include "EditorState.h"

//! [0]
// A Text editor widget for Picto Scripts
class LevelUpButton : public QToolButton
{
	Q_OBJECT
public: 
	LevelUpButton(QSharedPointer<EditorState> editorState, QWidget* parent = NULL);

private:
	QSharedPointer<EditorState> editorState_;
	QString origStyleSheet_;
private slots:
	void searchRequested(SearchRequest searchRequest);
	void windowAssetChanged(QSharedPointer<Asset> windowAsset);
};
































//! [0]
#endif
