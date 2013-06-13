#ifndef AssetExportImport_H
#define AssetExportImport_H

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include "../parameter/Analysis.h"
using namespace Picto;

//! [0]
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
	enum IMPORT_RESULT_TYPE{IMPORT_SUCCEEDED,IMPORT_SUCCEEDED_WITH_WARNINGS,IMPORT_FAILED,IMPORT_FAILED_ROLLBACK};
	QString exportToText(QList<QSharedPointer<Asset>> assets,AssociateRootHost* associateRootHost);
	int importFromText(QSharedPointer<Asset> pasteParent, QString pasteText, AssociateRootHost* experimentRootHost, QPoint pastePosition = QPoint(0,0), AssociateRootHost* analysisRootHost = NULL);
	QString getLatestMessage(){return latestMessage_;};

	enum TEXT_COMMAND_TYPE{NONE,EXPERIMENT_PASTE,ANALYSIS_PASTE,ANALYSIS_IMPORT};
	static int commandTypeOfText(QString text);
signals:
	void rollbackDesignRequired();	//A change to the design occured but failed, roll the design back to its previous state
private:
	QString latestMessage_;
	static bool getContentsUntilEndTag(QSharedPointer<QXmlStreamReader> xmlReader, QString endTag, QString& output);
};
//! [0]

#endif
