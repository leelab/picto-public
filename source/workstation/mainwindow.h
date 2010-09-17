#include <QMainWindow>
#include <QTextDocument>

#include "../common/experiment/experiment.h"

class QAction;
class Viewer;
class QTextDocument;
class ErrorList;
class QStackedWidget;

/*! \brief The main window for the Workstation tool
 *
 *	This window works similarly to the the Qt Creator tool.  There are multiple
 *	"Modes" which are accessed by a toolbar on the left of the window.  At the
 *	moment, there are only 3 modes: Edit, Test, and Run.  The edit mode provides an 
 *	XML editor for editing experiments (we'll need to add a GUI at some point in
 *	the near future).  The test mode allows you to run a state machine inside the
 *	Workstation interface, and the Run mode allows you to run an experiment on a 
 *	remote Director instance.
 *
 *	Workstation operates on a single "experiment" at a time.
 *
 *	The relationship between experiment_ and experimentText_ needs to be clariefied.
 *	experimentText_ is the XML that defines an experiment.  This is what all of our
 *	file operations operate on.  The experiment_ object is the actual experiment.  This
 *	distinction is important, since it's possible to have experimentText_ that can't
 *	be deserialized.  In this case, experiment_ will be null.
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow() {};

protected:
	void closeEvent(QCloseEvent *event);
signals:
	void error(QString errorType, QString errorText);

private slots:
	void newExperiment();
	void openExperiment();
	void openRecentExperiment();
	bool saveExperiment();
	bool saveAsExperiment();
	void changeMode();
	void checkSyntax();

private:
	void createActions();
	void createMenus();
	void createToolbars();
	void createViewers();

	void readSettings();
	void writeSettings();

	bool loadFile(const QString filename);
	bool saveFile(const QString filename);
	bool okToContinue();
	void setCurrentFile(const QString &filename);
	void updateRecentFileActions();
	bool convertTextToExperiment();


	QSharedPointer<Picto::Experiment> experiment_;
	QTextDocument experimentText_;

	ErrorList *errorList_;

	//File actions
	QAction *newExperimentAction_;
	QAction *openExperimentAction_;
	QAction *saveExperimentAction_;
	QAction *saveAsExperimentAction_;
	enum { MaxRecentFiles = 5 };
	QAction *recentExperimentsActions_[MaxRecentFiles];
	QAction *seperatorAction_;
	QAction *exitAction_;

	//Edit actions
	//QAction *cutAction_;
	//QAction *copyAction_;
	//QAction *pasteAction_;

	//Experiment Actions
	QAction *checkSyntaxAction_;


	//Menus
	QMenu* fileMenu_;
	//QMenu* editMenu_;
	QMenu* modeMenu_;
	QMenu* experimentMenu_;

	//Toolbars
	QToolBar *fileToolbar_;
	//QToolBar *editToolbar_;
	QToolBar *viewerToolbar_;

	//Viewers
	Viewer* currViewer_;
	QStackedWidget *viewerStack_;
	QList<QString> viewerNames_;

	//File info
	bool isModified_;
	QString currFile_;
	QStringList recentFiles_;
};