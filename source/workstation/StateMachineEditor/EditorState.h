#ifndef EDITORSTATE_H
#define EDITORSTATE_H

#include "../../common/storage/asset.h"
#include <QObject>
#include <QFont>
#include <QColor>
#include <QString>
#include <QGraphicsItem>
using namespace Picto;

class EditorState : public QObject
{
	Q_OBJECT
public:
	enum EditMode { Select, InsertLine };

	EditorState();

	//Get Functions
	EditMode getEditMode(){return editMode_;};
	QFont getFont(){return font_;};
	QColor getTextColor(){return textColor_;};
	QColor getItemColor(){return itemColor_;};
	QColor getLineColor(){return lineColor_;};
	QPixmap getBackgroundPattern(){return backgroundPattern_;};
	QString getInsertionItemCategory(){return insertItemCategory_;};
	QString getInsertionItemType(){return insertItemType_;};
	QSharedPointer<Asset> getWindowAsset(){return windowAsset_;};
	QSharedPointer<Asset> getSelectedAsset(){return selectedAsset_;};
	QGraphicsItem *getSelectedItem(){return selectedItem_;};

signals:
	void editModeChanged(EditMode mode);
	void fontChanged(QFont font);
	void textColorChanged(QColor color);
	void itemColorChanged(QColor color);
	void lineColorChanged(QColor color);
	void backgroundPatternChanged(QPixmap pattern);
	void insertionItemChanged(QString category, QString type);
	void windowAssetChanged(QSharedPointer<Asset> asset);
	void selectedAssetChanged(QSharedPointer<Asset> asset);
	void itemSelected(QGraphicsItem *item);
	void itemInserted();
	void undoableActionPerformed();
	void resetExperiment();

public slots:
	//Set Functions
	void setEditMode(EditMode mode){editMode_ = mode;emit editModeChanged(editMode_);};
	void setFont(const QFont font){font_ = font;emit fontChanged(font_);};
	void setTextColor(const QColor color){textColor_ = color;emit textColorChanged(textColor_);};
	void setItemColor(const QColor color){itemColor_ = color;emit itemColorChanged(itemColor_);};
	void setBackgroundPattern(QPixmap pattern){backgroundPattern_ = pattern;emit backgroundPatternChanged(backgroundPattern_);};
	void setLineColor(const QColor color){lineColor_ = color;emit lineColorChanged(lineColor_);};
	void setInsertionItem(QString category, QString type){insertItemCategory_ = category;insertItemType_ = type;emit insertionItemChanged(category,type);};
	void setWindowAsset(QSharedPointer<Asset> asset);
	void setSelectedAsset(QSharedPointer<Asset> asset);
	void setSelectedItem(QGraphicsItem *item);
	void setLastActionUndoable(){emit undoableActionPerformed();};
	void reloadWindow();
	void triggerExperimentReset(){emit resetExperiment();};
	void triggerItemInserted(){emit itemInserted();};

private:
	EditMode editMode_;
	QFont font_;
	QColor textColor_;
	QColor itemColor_;
	QColor lineColor_;
	QPixmap backgroundPattern_;
	QString insertItemCategory_;
	QString insertItemType_;
	QSharedPointer<Asset> windowAsset_;
	QSharedPointer<Asset> selectedAsset_;
	QGraphicsItem *selectedItem_;

};

#endif
