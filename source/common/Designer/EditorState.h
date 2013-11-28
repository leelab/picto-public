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

/*! \brief The EditorState stores the current state of the Designer and provides multiple signals that widgets throughout the Designer can connect to in order
 *	to be updated on important events.
 *	\details Since the Designer is built up of lots of different widgets that shouldn't need to know about each other, there needs to be some communication channel
 *	that allows each widget to communicate with others without needing to know what they do or what they need to know.  The EditorState is this communication channel.
 *	Whenever something significant happens, a widget just calls the appropriate function in the EditorState.  The EditorState emits the appropriate signals based on 
 *	this significant event and any widgets that connect to those signals will then be updated.  It also stores useful information like the EditorState::EditMode, 
 *	current Window Asset, current Analysis, current SearchRequest, etc.
 *	
 *	Since it is so central to Designer operation, the EditorState is passed into most if not all of the constructors of classes used by the Designer.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class EditorState : public QObject
{
	Q_OBJECT
public:
	/*! \brief The current Editing mode defining the effects of user interaction.*/
	enum EditMode 
	{ 
		Select = 0, //!< In this mode, the user can select items in the Designer canvas.  Their properties will appear in the Property window.
		PlaceItem,	//!< In this mode, the user can press the mouse button in the Designer canvas to insert the Asset type selected in the Toolbox into the current Window Asset.
		MoveItem,	//!< In this mode, while the mouse button is pressed, moving the mouse will move the currently selected item.
		InsertLine, //!< In this mode, pressing the mouse button will start drawing a new Transition Arrow.
		DrawLine,	//!< In this mode, while the mouse button is pressed, moving the mouse will continue drawing a Transition Arrow that was just created.
		Navigate	//!< In this mode, the user can rotate the mouse wheel to zoom the Designer canvas and press the mouse button and move it to drag the canvas.
	};

	EditorState();
	virtual ~EditorState(){};

	//Get Functions
	/*! \brief Returns the current EditorState::EditMode.
	 *	\sa EditorState::EditMode
	 */
	EditMode getEditMode(){return editMode_;};
	/*! \brief Returns the latest EditorState::EditMode before the current one.
	 *	\sa getEditMode()
	 */
	EditMode getLastEditMode(){return lastEditMode_;};
	/*! \brief Returns the font currently being used by the Picto Designer.
	 *	\note I believe that this function and setFont() are not currently being used.
	 */
	QFont getFont(){return font_;};
	/*! \brief Gets the text color currently being used by the Picto Designer.
	 *	\note I believe that this function and setTextColor() are not currently being used.
	 */
	QColor getTextColor(){return textColor_;};
	/*! \brief This function is not currently being used.
	 */
	QColor getItemColor(){return itemColor_;};
	/*! \brief Gets the line color currently being used by the Picto Designer.
	 *	\note I believe that this function and SetLineColor() are not currently being used.
	 */
	QColor getLineColor(){return lineColor_;};
	/*! \brief Gets the canvas background pattern currently being used by the Picto Designer.
	 *	\note I believe that this function and setBackgroundPattern() are not currently being used.
	 */
	QPixmap getBackgroundPattern(){return backgroundPattern_;};
	/*! \brief Returns the current insertion item category for the insertion item selected in the Toolbox.*/
	QString getInsertionItemCategory(){return insertItemCategory_;};
	/*! \brief Returns the current insertion item type for the insertion item selected in the Toolbox.*/
	QString getInsertionItemType(){return insertItemType_;};
	/*! \brief Returns the current insertion item diagram pixmap for the insertion item selected in the Toolbox.*/
	QPixmap getInsertionItemPixmap(){return insertItemPixmap_;};
	/*! \brief Returns the current WindowAsset, ie. The Asset whose children are displayed in the Designer canvas.*/
	QSharedPointer<Asset> getWindowAsset(){return windowAsset_;};
	/*! \brief Returns the current selected Asset, ie. the Asset which the selected DiagramItem symbolizes.*/
	QSharedPointer<Asset> getSelectedAsset(){return selectedAsset_;};
	/*! \brief Returns the current top level asset for this Designer.
	 *	\details The top level asset is the Asset whose children will be visible when the Designer won't let us navigate any further
	 *	up the design tree.  Currently, this is set by the Designer to be the Experiment Asset.
	 */
	QSharedPointer<Asset> getTopLevelAsset(){return topAsset_;};
	/*! \brief Returns the Analysis currently being displayed in the Designer, or an empty QSharedPointer<Analysis>
	 *	if the Designer is in Experimental design mode (ie. No Analysis Elements are visible and Experimental elements are editable).
	 */
	QSharedPointer<Analysis> getCurrentAnalysis(){return currAnalysis_;};
	/*! \brief Returns a QGraphicsItem pointer to the currently selected DiagramItem in the DiagramScene canvas.*/
	QGraphicsItem *getSelectedItem(){return selectedItem_;};
	QList<SearchRequest> getSearchRequests();

signals:
	/*! \brief Emitted when the EditorState::EditMode changes to a new value.  mode is the current EditorState::EditMode.*/
	void editModeChanged(int mode);
	/*! \brief Emitted when the zoom value for the canvas has changed.  zoom is the new zoom value.
	 *	\details The zoom value is essentially a constant by which the size of each object will be multiplied.
	 *	\sa setZoom()
	 */
	void zoomChanged(double zoom);
	/*! \brief Emitted when the font currently being used by the Picto Designer changes.
	 *	\note I believe that this signal, setFont(), and getFont() are not currently being used.
	 */
	void fontChanged(QFont font);
	/*! \brief Emitted when the text color currently being used by the Picto Designer changes.
	 *	\note I believe that this signal, setTextColor(), and getTextColor() are not currently being used.
	 */
	void textColorChanged(QColor color);
	/*! \brief This signal is not currently being used.
	 */
	void itemColorChanged(QColor color);
	/*! \brief Emitted when the line color currently being used by the Picto Designer changes.
	 *	\note I believe that this signal, setLineColor(), and getColor() are not currently being used.
	 */
	void lineColorChanged(QColor color);
	/*! \brief Emitted when the backgroundPattern currently being used by the Picto Designer changes.
	 *	\note I believe that this signal, getBackgrounPattern(), and setBackgrounPattern() are not currently being used.
	 */
	void backgroundPatternChanged(QPixmap pattern);
	/*! \brief Emitted when the insertion item selected in the Toolbox changes.
	 *	\details className is the name of the Asset's class, friendlyName is the friendlyName for the Asset's class that is shown
	 *	in the UI. 
	 */
	void insertionItemChanged(QString className,QString friendlyName);
	/*! \brief Emitted when all of the items in the current design window (ie. All of the DiagramItems and Arrows) have 
	 *	been successfully loaded.
	 *	\sa DiagramScene::setSceneAsset()
	 */
	void windowItemsLoaded();
	/*! \brief Emitted when the Window Asset changes.  asset is the new Window Asset.
	 *	\sa setWindowAsset()
	 */
	void windowAssetChanged(QSharedPointer<Asset> asset);
	/*! \brief Emitted when the Designer's currently selected Asset changes.  asset is the newly selected Asset.*/
	void selectedAssetChanged(QSharedPointer<Asset> asset);
	/*! \brief Emitted when the Designer's currently selected DiagramItem changes.  item is the newly selected DiagramItem.*/
	void itemSelected(QGraphicsItem *item);
	/*! \brief Emitted if the Analysis currently being displayed in the Designer changed.  currAnalysis is the new active Analysis.
	 *	\details If the Designer is switching to Experiment only mode, this signal will be emitted with an empty shared pointer.
	 */
	void currentAnalysisChanged(QSharedPointer<Analysis> currAnalysis);
	/*! \brief Emitted when the Designer's currently selected DiagramItem changes to an ArrowPortItem.  asset is the Asset referenced by the ArrowPortItem.*/
	void arrowPortSelected(QSharedPointer<Asset> asset);
	/*! \brief Emitted when a new DiagramItem or Arrow was just inserted into the Designer canvas.*/
	void itemInserted();
	/*! \brief Emitted when the latest group of actions since the last time this signal was emitted should be considered undoable
	 *	as a single unit.
	 */
	void undoableActionPerformed();
	/*! \brief Emitted when the latest group of changes marked by the undoableActionPerformed() signal should be undone.*/
	void undoRequested();
	/*! \brief Emitted when this experiment needs to be reloaded from XML.
	 *	\sa Designer::resetExperiment()
	 */
	void resetExperiment();
	/*! \brief Emitted when a new search is requested.  searchRequest is the new SearchRequest.
	 *	\details When emitted, all searchable elements in the Designer should serch for active searchRequests returned from getSearchRequests().
	 *	\sa SearchRequest
	 */
	void searchRequested(SearchRequest searchRequest);

public slots:
	//Set Functions
	void setTopLevelAsset(QSharedPointer<Asset> topLevelAsset);
	bool setCurrentAnalysis(QSharedPointer<Analysis> currAnalysis);
	void setEditMode(int mode);
	double setZoom(double zoom);
	/*! \brief Returns the font currently being used by the Picto Designer.
	 *	\note I believe that this function and getFont() are not currently being used.
	 */
	void setFont(const QFont font){font_ = font;emit fontChanged(font_);};
	/*! \brief Sets the text color currently being used by the Picto Designer.
	 *	\note I believe that this function and getTextColor() are not currently being used.
	 */
	void setTextColor(const QColor color){textColor_ = color;emit textColorChanged(textColor_);};
	/*! \brief This function is not currently being used.
	 */
	void setItemColor(const QColor color){itemColor_ = color;emit itemColorChanged(itemColor_);};
	/*! \brief Sets the canvas background pattern currently being used by the Picto Designer.
	 *	\note I believe that this function and getBackgroundPattern() are not currently being used.
	 */
	void setBackgroundPattern(QPixmap pattern){backgroundPattern_ = pattern;emit backgroundPatternChanged(backgroundPattern_);};
	/*! \brief Sets the line color currently being used by the Picto Designer.
	 *	\note I believe that this function and getLineColor() are not currently being used.
	 */
	void setLineColor(const QColor color){lineColor_ = color;emit lineColorChanged(lineColor_);};
	void setInsertionItem(QString className = "", QString friendlyName = "", QString category = "", QString type = "", QPixmap pixmap = QPixmap());
	void setWindowAsset(QSharedPointer<Asset> asset,bool undoable = true);
	void setWindowAssetToParent();
	/*! \brief Call this to inform the EditorState that all of the items in the current design window (ie. All of the DiagramItems and Arrows) have 
	 *	been successfully loaded.
	 *	\details This emits the windowItemsLoaded() signal when called.
	 *	\sa DiagramScene::setSceneAsset()
	 */
	void setWindowItemsLoaded(){emit windowItemsLoaded();};
	void setSelectedAsset(QSharedPointer<Asset> asset);
	void setSelectedItem(QGraphicsItem *item);
	/*! \brief Call this function to inform the EditorState that the latest group of actions since the last call to this function should be undoable
	 *	as a single unit.
	 *	\details This emits the undoableActionPerformed() signal when called.
	 */
	void setLastActionUndoable(){emit undoableActionPerformed();};
	void reloadWindow();
	/*! \brief Call to inform the EditorState that this experiment needs to be reloaded from XML.
	 *	\sa Designer::resetExperiment()
	 */
	void triggerExperimentReset(){emit resetExperiment();};
	/*! \brief Call to inform the EditorState that a new DiagramItem or Arrow was just inserted into the Designer canvas.*/
	void triggerItemInserted(){emit itemInserted();};
	/*! \brief Call this function to request that the latest group of changes marked by setLastActionUndoable() should be undone.*/
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
