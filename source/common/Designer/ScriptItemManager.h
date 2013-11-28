#ifndef SCRIPTITEMMANAGER_H
#define SCRIPTITEMMANAGER_H

#include <QMap>
#include <QVector>
#include <QColor>
#include "ScriptItem.h"

/*! \brief Manages creation and layout of ScriptItems for the various DiagramItems that use them.
 *	\details DiagramItems that show ScriptItems present them along their length in the order that
 *	the scripts are called with varying dimensions depending on whether we are operating in Analysis
 *	or Experiment mode.  This dramatically improves design readability, and since there are a number
 *	of separate DiagramItems that use this paradigm it is useful to create a single class that manages
 *	creation and layout of ScriptItems for reuse by all of the DiagramItems needing them.
 *	This is that class.  It shows Entry, Frame and Exit scripts depending on which Asset is being
 *	represented.  When operating in Analysis mode, Analysis scripts are shown as well.  Each script
 *	appears in the order that it is called (ie. AnalysisEntry, Entry, Frame, AnalysisFrame, Exit, 
 *	AnalysisExit) from top to bottom or left to right.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class ScriptItemManager : public QObject
{
	Q_OBJECT
public:
	ScriptItemManager(QSharedPointer<EditorState> editorState, QGraphicsItem *scriptsParent, QSharedPointer<Asset> asset, bool horizontal);
	virtual ~ScriptItemManager(){};
	virtual void setScriptBoundingRect(QRectF rect);
private:
	void updateScriptItemDimensions();
	/*! \brief The type of a particular script.*/
	enum SCRIPTTYPE
	{
		ANALYSISENTRY=0,	//!< An analysis script called just before a StateMachineElement's EntryScript during analysis.
		ENTRY,				//!< A run time script that is called after control reaches a StateMachineElement and its properties are reset to their init conditions.
		FRAME,				//!< A run time frame script that is called when a frame is rendered just before rendering that frame.
		ANALYSISFRAME,		//!< An analysis script that is called just when a frame is rendered just after rendering that frame during analysis.
		EXIT,				//!< An exit script that is called just before control flow leads a StateMachineElement
		ANALYSISEXIT,		//!< An analysis script this is called just after the Exit script during analysis.
		TYPE_NUM			//!< The total number of script types.
	};
	QVector<QString> orderedScriptNames_;
	QVector<QColor> scriptTypeColors_;
	float minScriptItemHeight_;
	QVector<float> scriptRegionStartPos_;
	QMap<int,ScriptItem*> scriptItems_;
	QSharedPointer<EditorState> editorState_;
	QSharedPointer<Asset> asset_;
	QRectF scriptBoundingRect_;
	bool horizontal_;
	QGraphicsItem* scriptsParent_;

private slots:
	void updateScriptItems();
	void performAnalysisScriptHolderOps(QSharedPointer<Asset> assetChild);
};


#endif
