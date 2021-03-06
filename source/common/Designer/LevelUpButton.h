#ifndef LEVELUPBUTTON_H
#define LEVELUPBUTTON_H

#include <QToolButton>
#include <QSharedPointer>
#include <QHash>
#include "EditorState.h"

/*! \brief A simple button widget containing an up arrow that turns red if the current Window Asset's ancestors contain the
 *	current search query.
 *	\details This widget is used by the Designer to switch the Window Asset to the parent of the current Window Asset.
 *	\sa EditorState::getWindowAsset(), EditorState::setWindowAsset(), EditorState::setWindowAssetToParent()
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class LevelUpButton : public QToolButton
{
	Q_OBJECT
public: 
	LevelUpButton(QSharedPointer<EditorState> editorState, QWidget* parent = NULL);

private:
	//! A pointer to the current EditorState
	QSharedPointer<EditorState> editorState_;
	//! A stylesheet string describin
	QString origStyleSheet_;
	//! A Hashtable of 
	QHash<int,bool> highlightByGroup_;
private slots:
	void searchRequested(SearchRequest searchRequest);
	void windowAssetChanged(QSharedPointer<Asset> windowAsset);
};

































#endif
