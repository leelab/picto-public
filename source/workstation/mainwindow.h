#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScriptEngine>
#include <QScriptEngineDebugger>

#include "ui_mainwindow.h"

#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"

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

    void about();

private slots:
    void updateActions();

	void valueChanged(QtProperty *changedProperty, const QVariant &value);

private:
    QUndoGroup *m_undoGroup;

    QString fixedWindowTitle(const Document *doc) const;

	void setupPropertyView();

	QtVariantPropertyManager *variantManager;
	QtVariantEditorFactory *variantFactory;
	QtTreePropertyBrowser *variantEditor;

    QScriptEngine * engine;
    QScriptEngineDebugger * debugger;
};

#endif // MAINWINDOW_H
