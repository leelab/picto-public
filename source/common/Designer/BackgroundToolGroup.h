#ifndef BACKGROUNDTOOLGROUP_H
#define BACKGROUNDTOOLGROUP_H
#include "ToolGroup.h"
/*! \brief An early class in Designer development that was going to allow us to change the canvas background in the designer...
 *	until we realized "who cares about the canvas background."
 *	\details This class is no longer used.  You may delete it if you want.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class BackgroundToolGroup : public ToolGroup
{
    Q_OBJECT

public:
   BackgroundToolGroup(QSharedPointer<EditorState> editorState,QWidget *parent=0);
   virtual ~BackgroundToolGroup(){};
protected:
   virtual void doButtonAction(int buttonId);
   virtual bool isEnabled(int){return true;};
};

#endif
