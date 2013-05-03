//#ifndef _DESIGN_H_
//#define _DESIGN_H_
//
//#include <QObject>
//#include <QTextDocument>
//#include "PictoData.h"
//
//
//namespace Picto{
//
///*!	\brief The error handling tool for Workstation
// *
// *	Eventually, this thing will be a full bown widget with a list of current
// *	errors.  However, since I'm still putting things together, it's going to 
// *	start off as a really simple list of errors, with basic access functions
// */
//#if defined WIN32 || defined WINCE
//class PICTOLIB_API Design : public QObject
//#else
//class Design : public QObject
//#endif
//{
//	Q_OBJECT
//public:
//	Design();
//	virtual ~Design(){};
//
////	bool resetRoot(QSharedPointer<UIEnabled> root);
//	//QSharedPointer<Asset> getRootAsset();
//	//QSharedPointer<Asset> getOpenAsset();
//	//void setOpenAsset(QSharedPointer<Asset> asset);
//	//void setUndoPoint();
//	//bool hasUndo();
//	//bool hasRedo();
//	//void undo();
//	//void redo();
////	void refreshFromXml();//Rebuilds experiment from XML.  Hopefully we'll be able to get rid of this sometime, currently, we need it for deleting assets which requires serializing then deserializing.
////	bool isModified();
////	void setUnmodified();
////	QString getDesignText();
////	bool compiles(QString* errors = &QString());
////
////signals:
////	void undoAvailable(bool available);
////	void redoAvailable(bool available);
//
//private:
//	QString updateAssetsFromText(QString designText);
//	QSharedPointer<Asset> findAssetWithID(QSharedPointer<Asset> root, int id);
//	static QSharedPointer<DataStore> createRoot(QString identifier);
//	QSharedPointer<UIEnabled> root_;
//	QSharedPointer<Asset> openAsset_;
//	//bool compiled_;
//	//QTextDocument designText_;
////
////private slots:
////	void designEdited();
//};
//};
//#endif