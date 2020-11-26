#ifndef DESIGNER_H
#define DESIGNER_H

#include <QMainWindow>
#include <QPointer>
#include <QButtonGroup>
#include <QTabWidget>

#include "SyntaxErrorBox.h"
#include "AboutElementBox.h"
#include "AnalysisOptionWidget.h"

#include "../storage/asset.h"

#include "diagramitem.h"
#include "EditorState.h"
#include "../design/DesignRoot.h"
#include "LevelUpButton.h"
#include "../common.h"
using namespace Picto;

class DiagramScene;
class Toolbox;

QT_BEGIN_NAMESPACE
class QAction;
class QToolBox;
class QSpinBox;
class QComboBox;
class QFontComboBox;
class QButtonGroup;
class QLineEdit;
class QGraphicsTextItem;
class QFont;
class QToolButton;
class QAbstractButton;
class QGraphicsView;
class QCheckBox;
class QFrame;
QT_END_NAMESPACE

/*! \brief A Graphical State Machine Integrated Development Environment for creating Picto Experiments and Analyses.
*	\details This is the top level widget of the Picto Graphical State Machine IDE for creating Picto designs.
*	The Designer includes a central canvas where StateMachineElements, Variables, Stimuli, AnalysisElements, etc can be
*	dragged and dropped and linked together with "Arrow" transitions.
*
*	On the right hand side is a PropertyBrowser that displays the properties defining the currently selected Asset.
*
*	On the left hand side is a contextually aware Toolbox that provides drag-and-drop tools for creating all elements that
*	are addable in the current designer context.
*
*	Underneath the Picto canvas is an information box including tabs to display the results of the latest compilation,
*	information about the currently selected element in the canvas or toolbox, and contextual scripting information
*	listing all of the script properties and functions available from the currently selected element.
*
*	Underneath the PropertyBrowser is an ElementNotesWidget that displays any notes that the user added describing the
*	currently selected experimental element (these note also appear in the element's tooltip).
*
*	A Toolbar appears at the top of the screen providing copy/paste, undo/redo, search and compilation functionality.
*	Analyses can also be created/selected from this Toolbar and whether or not an Analysis is selected changes the
*	Designer context as to whether Experimental elements are editable or not along with whether Experimental or Analysis
*	elements appear in the left hand ToolBox.
*
*	Context menus are also provided for all elements that contain simple operations like copy/paste.
*
*	In the designer, the mouse is contextually aware.  When hovering over an elements body, clicking the mouse selects
*	the element.  When hovering over a result bar, clicking the mouse starts creating a transition, etc.
*
*	In essence, the designer is a GUI for interacting with the DesignRoot loaded in loadDesign.  A design could be
*	created entirely in a text editor, but the UI makes things astronomically easier.
*	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
*	\date 2009-2015
*/
#if defined WIN32 || defined WINCE
class PICTOLIB_API Designer : public QWidget
#else
class Designer : public QWidget
#endif
{
	Q_OBJECT

public:
	Designer(QWidget *parent = 0);
	virtual ~Designer();

	void loadDesign(QSharedPointer<DesignRoot> designRoot);
	void activate(bool enable);

	private slots:
	void sceneScaleChanged(const QString &scale);
	void sceneScaleChanged(double scale);
	void sceneScaleChanged();
	void loadScene(DiagramScene* newScene);
	void resetExperiment();
	void insertEditBlock();
	void performUndoAction();
	void performRedoAction();
	void setOpenAsset(QSharedPointer<Asset> asset);
	void selectedAssetChanged(QSharedPointer<Asset> asset);
	void selectedItemChanged(QGraphicsItem *item);
	void currentAnalysisChanged(QSharedPointer<Analysis> analysis);
	void checkSyntax();

private:
	void createActions();
	void connectActions();
	void createMenus();
	void createToolbars();
	bool resetEditor();
	void updateEnabledActions();

	QSharedPointer<DesignRoot> designRoot_;
	QSharedPointer<EditorState> editorState_;
	DiagramScene *scene;
	DiagramScene *topmostScene;
	Toolbox *toolbox_;
	QGraphicsView *view;

	QAction *exitAction;
	QAction *addAction;
	QAction *deleteAction;
	QAction *copyAction;
	QAction *analysisExportAction;
	QAction *analysisImportAction;
	QAction *pasteAction;
	QLineEdit *searchBox;
	QCheckBox *matchCase;
	QFrame* searchWidget;
	QAction *checkSyntaxAction_;

	QAction *undoAction;
	QAction *redoAction;

	AnalysisOptionWidget *analysisOption_;
	int selectedIndex_;

	QMenu *fileMenu;
	QMenu *itemMenu;
	QMenu *sceneMenu;

	QToolBar *editToolBar;
	QToolBar *pointerToolbar;

	QComboBox *sceneScaleCombo;
	QSpinBox *sceneScaleSpin;
	int sceneScaleValue_;

	QSharedPointer<QButtonGroup> buttonGroup;
	QSharedPointer<QButtonGroup> backgroundButtonGroup;
	QToolButton *fillColorToolButton;
	QToolButton *lineColorToolButton;
	QAction *textAction;
	QAction *fillAction;
	QAction *lineAction;

	LevelUpButton* upButton;
	QWidget *propertyEditor_;
	QWidget *notesWidget_;

	enum INFOTABS{ ABOUTELEMENT, SCRIPTINFO, SYNTAXCHECK };
	QWidget *assetInfoBox_;
	SyntaxErrorBox *syntaxErrorBox_;
	AboutElementBox *aboutElementBox_;
	QTabWidget *infoPane_;

	private slots:
	void undoAvailable(bool available);
	void redoAvailable(bool available);
	void searchTextChanged(const QString& text);
	void matchCaseChanged(int state);

};


#endif
