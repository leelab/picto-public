#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScriptEngine>
#include <QScriptEngineDebugger>
#include <QUuid>

#include "ui_mainwindow.h"

#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"

#include "../common/network/CommandChannel.h"
#include "../common/network/ServerDiscoverer.h"

#include "../common/experiment/experiment.h"

class Document;

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
	~MainWindow();

    void addDocument(Document *doc);
    void removeDocument(Document *doc);
    void setCurrentDocument(Document *doc);
    Document *currentDocument() const;

public slots:
    void openDocument();
    void saveDocument();
    void closeDocument(int index = -1);
    void newDocument();

    void addShape();
    void removeShape();
    void toggleClipping();
	void beginDebug();

	void startSession();
	void runTask();

    void about();

private slots:
    void updateActions();

	void valueChanged(QtProperty *changedProperty, const QVariant &value);

	void connectToServer();

private:
    QUndoGroup *m_undoGroup;

    QString fixedWindowTitle(const Document *doc) const;

	void setupPropertyView();

	QtVariantPropertyManager *variantManager;
	QtVariantEditorFactory *variantFactory;
	QtTreePropertyBrowser *variantEditor;

    QScriptEngine *engine;
    QScriptEngineDebugger *debugger;

	QSharedPointer<Picto::CommandChannel> serverChannel_;
	Picto::ServerDiscoverer *serverDiscoverer_;

	QSharedPointer<Picto::Experiment> experiment_;
	QUuid sessionId_;
};

#endif // MAINWINDOW_H
