/*!	\brief	This views the experiment as a simple XML text file.
 *
 *	The viewer includes a simple text editor (which may be expanded as a true XML
 *	editor at some point, but currently is just a dumb editor).
 *
 *	Potential future features:
 *		- Syntax highlighting (easy, but I'd have to figure out
 *		  regexes for XML)
 *		- Outlining (like in VS) this may be tough.
 *		- Go to matching tag command that can be run without the mouse.
 */
#ifndef _TEXT_VIEWER_H_
#define _TEXT_VIEWER_H_

#include "viewer.h"
#include "xmleditor.h"

class QTextEdit;
class QVBoxLayout;
class QToolBar;
class QComboBox;
class QLabel; 

class TextViewer : public Viewer
{
	Q_OBJECT
public:
	TextViewer(QAction* checkSyntaxAction, QWidget *parent=0);
	QString type() { return "Text"; };

public slots:
	void init();  //Called just before displaying the viewer
	void deinit();	//Called just after the user switches out of the viewer

private slots:
	void goToTask(QString taskname);
	void updateStatus();

private:
	void fillTaskComboBox();

	QVBoxLayout *mainLayout_;
	XmlEditor *xmlEdit_;
	
	QComboBox *taskComboBox_;
	QToolBar *editToolBar_;
	QLabel *statusLabel_;


};

#endif