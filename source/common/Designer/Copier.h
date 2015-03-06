#ifndef Copier_H
#define Copier_H

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include "EditorState.h"
#include "../storage/AssetExportImport.h"
using namespace Picto;

/*! \brief Uses the AssetExportImporter to implement copy/paste functionality within a Picto design.
 *	\details This class implements copy/paste using the standard text clipboard.  During a copy, the 
 *	text exported from the AssetExportImporter is saved to the standard computer clipboard, during 
 *	paste, text from the clipboard is read, checked for Picto import compatibility and if compatible
 *	imported.  In terms of its actual functionality, this class is not much more than a wrapper for
 *	the AssetExportImporter that adds clipboard functionality.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class Copier : public QObject
{
	Q_OBJECT
public:
	Copier(QSharedPointer<EditorState> editorState);
	virtual ~Copier();

	void copy(QList<QSharedPointer<Asset>> assets,bool copyAnalysis = false);
	void paste(QSharedPointer<Asset> pasteParent, QPoint pastePosition = QPoint(0,0));

	/*! \brief An enum describing the types of paste operations available for the string currently in the computer's clipboard.*/
	enum PASTE_TYPE{
		NONE,				//!< Exported String cannot be imported
		EXPERIMENT_PASTE,	//!< Exported String is made up of experimental assets that can be pasted into an experiment
		ANALYSIS_PASTE,		//!< Exported String is made up of analysis assets on a single level in the design tree that can be pasted onto a window Asset in the designer
		ANALYSIS_IMPORT		//!< Exported String is made up of analysis assets from multiple levels of the design tree that can be imported into a StateMachineElement (by right clicking on that element)
	};
	static int availablePasteType();
private:
	QSharedPointer<EditorState> editorState_;
	AssetExportImport exportImporter_;
};


#endif
