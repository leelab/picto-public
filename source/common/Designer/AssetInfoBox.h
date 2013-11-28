#ifndef ASSETINFOBOX_H
#define ASSETINFOBOX_H
#include "EditorState.h"
#include "SearchableTextEdit.h"

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

/*! \brief Provides data about scipts that are in scope for the currently selected Asset in the Designer.
 *	\note This widget is much less useful now that we have th AboutElementBox.  We might think about
 *	either removing it or updating it with the additional data available from the AssetDescriber.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
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

#endif
