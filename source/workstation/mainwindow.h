#include <QMainWindow>
#include <QTextDocument>

#include "../common/experiment/experiment.h"
#include "../common/design/pictodata.h"
#include "../common/design/designRoot.h"
using namespace Picto;

class QAction;
class Viewer;
class QTextDocument;
class ErrorList;
class QStackedWidget;

/*! \brief The main window for the Workstation app.
 *
 *	\details This window includes multiple
 *	"Modes" which are accessed by a toolbar on the left of the window.  At the moment, the modes are:  
 *	- A StateMachineEditor - A graphical state machine based development environment that is used to design Picto
 *			Experiments and their Analysis.
 *	- A TestViewer - A test environment where Experiments and Analyses can be run and debugged before being deployed in a
 *			real experimental Session.
 *	- A RemoteViewer - A control panel that is used to start an experimental session on a Director/Proxy system, view it
 *			in real time, control its operation, and interact with the experiment subject.
 *	- A ReplayViewer - A playback system that allows for replaying completed experimental Sessions, recording their
 *			activity to video, running Analysis on the Sessions, and saving the Analysis output to disk. 
 *
 *	While each of these Workstation modes functions to a great extent in its own world, there are some loose ties between
 *	all Viewers.  The main tie between all of these viewers is a "current design file."  The Workstation as whole always
 *	has a design file loaded.  The Design stored in the design file's XML has different functions in each Viewer.  In the
 *	StateMachineEditor, the design is what is being created and edited.  In the TestViewer the design is what is being run
 *	and tested.  In the RemoteViewer the design is what is being sent over to a Director in order to run an experimental
 *	Session.  In the ReplayViewer, the design defines the Analyses that can be imported into the currently loaded Session
 *	files in order to gather data that may not have been considered when a Session was first run.
 *
 *	This window also includes some simple menus with functionality for saving the current Design, opening a new Design
 *	from different types of files, changing the System number (Picto applications only communicate with the Server that
 *	has their System number) and retrieving broad "about" information on the Picto system.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QColor);
	virtual ~MainWindow() {};

protected:
	void closeEvent(QCloseEvent *event);
signals:
	/*! \brief This does not appear to be used.*/
	void error(QString errorType, QString errorText);

private slots:
	void newExperiment();
	void openExperiment();
	void openRecentExperiment();
	void openDesign(QSharedPointer<DesignRoot> designRoot);
	bool saveExperiment();
	bool saveAsExperiment();
	void changeMode();
	void startMode();
	//void checkSyntax();
	void changeSystemNumber();
	void aboutPicto();

private:
	void changeMode(Viewer* nextViewer);
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
	bool convertTextToPictoData();


	//QSharedPointer<Picto::PictoData> pictoData_;
	//QTextDocument pictoDataText_;
	QSharedPointer<DesignRoot> designRoot_;

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
	QAction *initViewerAction_;
	QAction *setSystemNumberAction_;
	QAction *aboutPictoAction_;


	//Menus
	QMenu* fileMenu_;
	//QMenu* editMenu_;
	QMenu* modeMenu_;
	QMenu* systemMenu_;
	QMenu* experimentMenu_;
	QMenu* aboutMenu_;

	//Toolbars
	QToolBar *fileToolbar_;
	//QToolBar *editToolbar_;
	QToolBar *viewerToolbar_;

	//Viewers
	Viewer* currViewer_;
	Viewer* nextViewer_;
	QStackedWidget *viewerStack_;
	QList<QString> viewerNames_;

	//File info
	bool isModified_;
	QString currFile_;
	QStringList recentFiles_;

	QColor bgCol_;
};