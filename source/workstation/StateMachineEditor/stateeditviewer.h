#ifndef STATEEDITVIEWER_H
#define STATEEDITVIEWER_H

#include "../../common/Designer/AutoSaver.h"
#include "../viewer.h"
#include "../../common/Designer/Designer.h"
using namespace Picto;

/*! \brief The StateEditViewer is a container for the Designer widget which is a development environment
 *	for creating Picto Experiments and Analyses.
 *	\details Apart from being a container for the Designer, this class doesn't do much.  It really only
 *	implements init() and deinit() functions to take care of a number of simple operations whenever this
 *	Viewer takes and leaves focus.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class StateEditViewer : public Viewer
{
    Q_OBJECT

public:
   StateEditViewer(QWidget *parent=0);
   virtual ~StateEditViewer();
   virtual QString type(){return "State Edit Viewer";};
signals:
   void loadDesignRoot(QSharedPointer<DesignRoot> designRoot);
public slots:
	virtual void init();
	virtual void deinit();
	virtual void aboutToSave();

private:

	Designer* expDesigner_;
	QSharedPointer<AutoSaver> autoSaver_;
	QTimer autoSaveTimer_;
	bool checkedAutoSave_;
};


#endif
