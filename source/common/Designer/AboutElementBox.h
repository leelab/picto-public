#ifndef AboutElementBox_H
#define AboutElementBox_H
#include "EditorState.h"
#include "SearchableTextEdit.h"
#include "AssetDescriber.h"

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

/*! \brief A searchable read-only text box that provides information about Picto Asset functionality.
 *	\details This box is used in the picto Designer to provide desginers with contextual data
 *	including an Overview, DataStore properties, Script properties, and Script functions for every
 *	selected Asset.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
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

#endif
