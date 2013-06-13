#ifndef Copier_H
#define Copier_H

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include "EditorState.h"
#include "../storage/AssetExportImport.h"
using namespace Picto;

//! [0]
class Copier : public QObject
{
	Q_OBJECT
public:
	Copier(QSharedPointer<EditorState> editorState);
	virtual ~Copier();

	void copy(QList<QSharedPointer<Asset>> assets,bool copyAnalysis = false);
	void paste(QSharedPointer<Asset> pasteParent, QPoint pastePosition = QPoint(0,0));

	enum PASTE_TYPE{NONE,EXPERIMENT_PASTE,ANALYSIS_PASTE,ANALYSIS_IMPORT};
	static int availablePasteType();
private:
	QSharedPointer<EditorState> editorState_;
	AssetExportImport exportImporter_;
};
//! [0]

#endif
