#ifndef AssetExportImport_H
#define AssetExportImport_H

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include "../parameter/Analysis.h"
using namespace Picto;

/*! \brief Provides functionality for exporting selected assets and importing them into a separate experiment or the same
 *	experiment at a different location.
 *	\details This class is used for copy/paste functionality and larger scale importing of entire Analyses into new
 *	experiments during the design process or to Analyze a session that didn't have an appropriate Analysis saved with the
 *	design when the session was run.  With this class, Experimental and Analysis elements must be exported separately.
 *	Since Analysis elements don't define a tree structure but are added into an experimental tree structure, import of
 *	analysis elements can be partially succesful where some Analyses find their correct parent elements and some don't.
 *	In these cases, Analysis elements that can be added are added, and those that can't are listed in a returned message.
 *	It is also possible for the import to fail but with a changed design.  In these cases, the return value from the
 *	importFromText() function will indicate this, and a rollbackDesignRequired() signal will be triggered as well.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API AssetExportImport : public QObject
#else
class AssetExportImport : public  QObject
#endif
{
	Q_OBJECT
public:
	AssetExportImport();
	virtual ~AssetExportImport();
	/*! \brief An enum describing the result of an import process. */
	enum IMPORT_RESULT_TYPE{
		IMPORT_SUCCEEDED,	//!< Indicates that the import succeeded
		IMPORT_SUCCEEDED_WITH_WARNINGS,	//!< Indicates that the import succeeded but there were some warnings
		IMPORT_FAILED,	//!<	Indicates that the import failed, but the design hasn't been changed.
		IMPORT_FAILED_ROLLBACK	//!< Indicates that the import failed and the design has been changed.  An Undo should be used to return it to its prior state.
	};
	QString exportToText(QList<QSharedPointer<Asset>> assets,AssociateRootHost* associateRootHost);
	int importFromText(QSharedPointer<Asset> pasteParent, QString pasteText, AssociateRootHost* experimentRootHost, QPoint pastePosition = QPoint(0,0), AssociateRootHost* analysisRootHost = NULL);
	/*! \brief When something didn't work, this returns the latest messages describing what.
	 */ 
	QString getLatestMessage(){return latestMessage_;};

	/*! \brief An enum describing the types of operations available for a given string exported from exportToText()
	 */
	enum TEXT_COMMAND_TYPE{
		NONE,				//!< Exported String cannot be imported
		EXPERIMENT_PASTE,	//!< Exported String is made up of experimental assets that can be pasted into an experiment
		ANALYSIS_PASTE,		//!< Exported String is made up of analysis assets on a single level in the design tree that can be pasted onto a window Asset in the designer
		ANALYSIS_IMPORT		//!< Exported String is made up of analysis assets from multiple levels of the design tree that can be imported into a StateMachineElement (by right clicking on that element)
	};
	static int commandTypeOfText(QString text);
signals:
	//!	A change to the design occured but failed, roll the design back to its previous state
	void rollbackDesignRequired();	
private:
	QString latestMessage_;
	static bool getContentsUntilEndTag(QSharedPointer<QXmlStreamReader> xmlReader, QString endTag, QString& output);
};


#endif
