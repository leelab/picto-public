#ifndef BACKGROUNDTOOLGROUP_H
#define BACKGROUNDTOOLGROUP_H
#include "ToolGroup.h"

class BackgroundToolGroup : public ToolGroup
{
    Q_OBJECT

public:
   BackgroundToolGroup(QSharedPointer<EditorState> editorState,QWidget *parent=0);
protected:
   virtual void doButtonAction(int buttonId);
};

#endif
