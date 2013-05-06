#ifndef EDITORSTATE_H
#define EDITORSTATE_H

#include "../../common/storage/asset.h"
#include <QObject>
#include <QFont>
#include <QColor>
#include <QString>
#include <QGraphicsItem>
#include "../../common/design/PictoData.h"
#include "../storage/SearchRequest.h"
#include "../parameter/Analysis.h"
using namespace Picto;

class EditorState : public QObject
{
	Q_OBJECT
public:
	enum EditMode { Select = 0, PlaceItem, MoveItem, InsertLine, DrawLine, Navigate};

	EditorState();
	virtual ~EditorState(){};

	//Get Functions
	EditMode getEditMode(){return editMode_;};
	EditMode getLastEditMode(){return lastEditMode_;};
	QFont getFont(){return font_;};
	QColor getTextColor(){return textColor_;};
	QColor getItemColor(){return itemColor_;};
	QColor getLineColor(){return lineColor_;};
	QPixmap getBackgroundPattern(){return backgroundPattern_;};
	QString getInsertionItemCategory(){return insertItemCategory_;};
	QString getInsertionItemType(){return insertItemType_;};
	QPixmap getInsertionItemPixmap(){return insertItemPixmap_;};
	QSharedPointer<Asset> getWindowAsset(){return windowAsset_;};
	QSharedPointer<Asset> getSelectedAsset(){return selectedAsset_;};
	QSharedPointer<Asset> getTopLevelAsset(){return topAsset_;};
	QSharedPointer<Analysis> getCurrentAnalysis(){return currAnalysis_;};
	QGraphicsItem *getSelectedItem(){return selectedItem_;};
	QList<SearchRequest> getSearchRequests();

signals:
	void editModeChanged(int mode);
	void zoomChanged(double zoom);
	void fontChanged(QFont font);
	void textColorChanged(QColor color);
	void itemColorChanged(QColor color);
	void lineColorChanged(QColor color);
	void backgroundPatternChanged(QPixmap pattern);
	void insertionItemChanged(QString className,QString friendlyName);
	void windowItemsLoaded();
	void windowAssetChanged(QSharedPointer<Asset> asset);
	void selectedAssetChanged(QSharedPointer<Asset> asset);
	void itemSelected(QGraphicsItem *item);
	void currentAnalysisChanged(QSharedPointer<Analysis> currAnalysis);
	void arrowPortSelected(QSharedPointer<Asset> asset);
	void itemInserted();
	void undoableActionPerformed();
	void undoRequested();
	void resetExperiment();
	void searchRequested(SearchRequest searchRequest);

public slots:
	//Set Functions
	void setTopLevelAsset(QSharedPointer<Asset> topLevelAsset);
	bool setCurrentAnalysis(QSharedPointer<Analysis> currAnalysis);
	void setEditMode(int mode);
	double setZoom(double zoom);
	void setFont(const QFont font){font_ = font;emit fontChanged(font_);};
	void setTextColor(const QColor color){textColor_ = color;emit textColorChanged(textColor_);};
	void setItemColor(const QColor color){itemColor_ = color;emit itemColorChanged(itemColor_);};
	void setBackgroundPattern(QPixmap pattern){backgroundPattern_ = pattern;emit backgroundPatternChanged(backgroundPattern_);};
	void setLineColor(const QColor color){lineColor_ = color;emit lineColorChanged(lineColor_);};
	void setInsertionItem(QString className = "", QString friendlyName = "", QString category = "", QString type = "", QPixmap pixmap = QPixmap());
	void setWindowAsset(QSharedPointer<Asset> asset,bool undoable = true);
	void setWindowAssetToParent();
	void setWindowItemsLoaded(){emit windowItemsLoaded();};
	void setSelectedAsset(QSharedPointer<Asset> asset);
	void setSelectedItem(QGraphicsItem *item);
	void setLastActionUndoable(){emit undoableActionPerformed();};
	void reloadWindow();
	void triggerExperimentReset(){emit resetExperiment();};
	void triggerItemInserted(){emit itemInserted();};
	void requestUndo(){emit undoRequested();};
	void requestSearch(SearchRequest searchRequest);

private:
	void setWindowAssetAncestryOpen(bool open);
	EditMode editMode_;
	EditMode lastEditMode_;
	QFont font_;
	QColor textColor_;
	QColor itemColor_;
	QColor lineColor_;
	QPixmap backgroundPattern_;
	QString insertItemCategory_;
	QString insertItemType_;
	QPixmap insertItemPixmap_;
	QSharedPointer<Asset> windowAsset_;
	QSharedPointer<Asset> selectedAsset_;
	QString windowAssetPath_;
	QGraphicsItem *selectedItem_;
	QSharedPointer<Asset> topAsset_;
	QSharedPointer<Analysis> currAnalysis_;
	QHash<int,SearchRequest> searchRequests_;

};

#endif
