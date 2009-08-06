#include <QUndoGroup>
#include <QUndoStack>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QToolButton>
#include <QDate>
#include <QScriptEngineDebugger>
#include <QScriptEngine>

#include "document.h"
#include "mainwindow.h"
#include "commands.h"

#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
	  variantManager(NULL),
	  variantFactory(NULL),
	  variantEditor(NULL)
{
    setupUi(this);

    QWidget *w = documentTabs->widget(0);
    documentTabs->removeTab(0);
	documentTabs->setTabsClosable(true);
	documentTabs->setMovable(true);
    delete w;

    connect(actionOpen, SIGNAL(triggered()), this, SLOT(openDocument()));
	connect(documentTabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeDocument(int)));
    connect(actionNew, SIGNAL(triggered()), this, SLOT(newDocument()));
    connect(actionSave, SIGNAL(triggered()), this, SLOT(saveDocument()));
    connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(actionAddCircle, SIGNAL(triggered()), this, SLOT(addShape()));
    connect(actionAddRectangle, SIGNAL(triggered()), this, SLOT(addShape()));
    connect(actionAddTriangle, SIGNAL(triggered()), this, SLOT(addShape()));
    connect(actionRemoveShape, SIGNAL(triggered()), this, SLOT(removeShape()));
    connect(actionToggleClipping, SIGNAL(triggered()), this, SLOT(toggleClipping()));
    connect(actionDebug, SIGNAL(triggered()), this, SLOT(beginDebug()));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    connect(documentTabs, SIGNAL(currentChanged(int)), this, SLOT(updateActions()));

    actionOpen->setShortcut(QString("Ctrl+O"));
    actionNew->setShortcut(QString("Ctrl+N"));
    actionSave->setShortcut(QString("Ctrl+S"));
    actionExit->setShortcut(QString("Ctrl+Q"));
    actionRemoveShape->setShortcut(QString("Del"));
    actionToggleClipping->setShortcut(QString("Alt+C"));
    actionAddCircle->setShortcut(QString("Alt+R"));
    actionAddRectangle->setShortcut(QString("Alt+L"));
    actionAddTriangle->setShortcut(QString("Alt+T"));

    m_undoGroup = new QUndoGroup(this);
    undoView->setGroup(m_undoGroup);
    undoView->setCleanIcon(QIcon(":/icons/filesave.png"));

    QAction *undoAction = m_undoGroup->createUndoAction(this);
    QAction *redoAction = m_undoGroup->createRedoAction(this);
    undoAction->setIcon(QIcon(":/icons/undo.png"));
    redoAction->setIcon(QIcon(":/icons/redo.png"));
    menuShape->insertAction(menuShape->actions().at(0), undoAction);
    menuShape->insertAction(undoAction, redoAction);

    toolBar->addAction(undoAction);
    toolBar->addAction(redoAction);

    newDocument();
    updateActions();

	setupPropertyView();

    engine = new QScriptEngine();
    debugger = new QScriptEngineDebugger();
    debugger->attachTo(engine);
	debugger->setAutoShowStandardWindow(false);

	QWidget *codeWindow = debugger->widget(QScriptEngineDebugger::CodeWidget);

    QDockWidget *dockWidget;

	dockWidget = new QDockWidget("Breakpoints", this);
	dockWidget->setObjectName("Breakpoints");
	dockWidget->setWidget(debugger->widget(QScriptEngineDebugger::BreakpointsWidget));

	addDockWidget(Qt::LeftDockWidgetArea,dockWidget);

	dockWidget = new QDockWidget("Local Variables", this);
	dockWidget->setObjectName("Local Variables");
	dockWidget->setWidget(debugger->widget(QScriptEngineDebugger::LocalsWidget));

	addDockWidget(Qt::BottomDockWidgetArea,dockWidget);

	dockWidget = new QDockWidget("Call Stack", this);
	dockWidget->setObjectName("Call Stack");
	dockWidget->setWidget(debugger->widget(QScriptEngineDebugger::StackWidget));

	addDockWidget(Qt::BottomDockWidgetArea,dockWidget);

	QAction *continueAction = debugger->action(QScriptEngineDebugger::ContinueAction);
	QAction *stepOverAction = debugger->action(QScriptEngineDebugger::StepOverAction);
	QAction *stepIntoAction = debugger->action(QScriptEngineDebugger::StepIntoAction);

    continueAction->setShortcut(QString("F5"));
    stepIntoAction->setShortcut(QString("F11"));
    stepOverAction->setShortcut(QString("F10"));

	QToolBar *toolBar = new QToolBar;
	toolBar->addAction(continueAction);
	toolBar->addAction(stepOverAction);
	toolBar->addAction(stepIntoAction);

	addToolBar(toolBar);

    documentTabs->addTab(codeWindow, "Script");
}

MainWindow::~MainWindow()
{
    if(variantManager) delete variantManager;
    if(variantFactory) delete variantFactory;
    if(variantEditor) delete variantEditor;
	if(engine) delete engine;
	if(debugger) delete debugger;
}

void MainWindow::setupPropertyView()
{
	variantManager = new QtVariantPropertyManager();

    int i = 0;
    QtProperty *topItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),
                QString::number(i++) + QLatin1String(" Rectangle Properties"));
	QtVariantProperty *item;
/*
    QtVariantProperty *item = variantManager->addProperty(QVariant::Bool, QString::number(i++) + QLatin1String(" Bool Property"));
    item->setValue(true);
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Int, QString::number(i++) + QLatin1String(" Int Property"));
    item->setValue(20);
    item->setAttribute(QLatin1String("minimum"), 0);
    item->setAttribute(QLatin1String("maximum"), 100);
    item->setAttribute(QLatin1String("singleStep"), 10);
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Double, QString::number(i++) + QLatin1String(" Double Property"));
    item->setValue(1.2345);
    item->setAttribute(QLatin1String("singleStep"), 0.1);
    item->setAttribute(QLatin1String("decimals"), 3);
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::String, QString::number(i++) + QLatin1String(" String Property"));
    item->setValue("Value");
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Date, QString::number(i++) + QLatin1String(" Date Property"));
    item->setValue(QDate::currentDate().addDays(2));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Time, QString::number(i++) + QLatin1String(" Time Property"));
    item->setValue(QTime::currentTime());
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::DateTime, QString::number(i++) + QLatin1String(" DateTime Property"));
    item->setValue(QDateTime::currentDateTime());
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::KeySequence, QString::number(i++) + QLatin1String(" KeySequence Property"));
    item->setValue(QKeySequence(Qt::ControlModifier | Qt::Key_Q));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Char, QString::number(i++) + QLatin1String(" Char Property"));
    item->setValue(QChar(386));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Locale, QString::number(i++) + QLatin1String(" Locale Property"));
    item->setValue(QLocale(QLocale::Polish, QLocale::Poland));
    topItem->addSubProperty(item);
*/
    item = variantManager->addProperty(QVariant::Point, QString::number(i++) + QLatin1String(" Position"));
    item->setValue(QPoint(10, 10));
    topItem->addSubProperty(item);
/*
    item = variantManager->addProperty(QVariant::PointF, QString::number(i++) + QLatin1String(" PointF Property"));
    item->setValue(QPointF(1.2345, -1.23451));
    item->setAttribute(QLatin1String("decimals"), 3);
    topItem->addSubProperty(item);
*/
    item = variantManager->addProperty(QVariant::Size, QString::number(i++) + QLatin1String(" Size"));
    item->setValue(QSize(20, 20));
    item->setAttribute(QLatin1String("minimum"), QSize(10, 10));
    item->setAttribute(QLatin1String("maximum"), QSize(30, 30));
    topItem->addSubProperty(item);
/*
    item = variantManager->addProperty(QVariant::SizeF, QString::number(i++) + QLatin1String(" SizeF Property"));
    item->setValue(QSizeF(1.2345, 1.2345));
    item->setAttribute(QLatin1String("decimals"), 3);
    item->setAttribute(QLatin1String("minimum"), QSizeF(0.12, 0.34));
    item->setAttribute(QLatin1String("maximum"), QSizeF(20.56, 20.78));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Rect, QString::number(i++) + QLatin1String(" Rect Property"));
    item->setValue(QRect(10, 10, 20, 20));
    topItem->addSubProperty(item);
    item->setAttribute(QLatin1String("constraint"), QRect(0, 0, 50, 50));

    item = variantManager->addProperty(QVariant::RectF, QString::number(i++) + QLatin1String(" RectF Property"));
    item->setValue(QRectF(1.2345, 1.2345, 1.2345, 1.2345));
    topItem->addSubProperty(item);
    item->setAttribute(QLatin1String("constraint"), QRectF(0, 0, 50, 50));
    item->setAttribute(QLatin1String("decimals"), 3);

    item = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(),
                    QString::number(i++) + QLatin1String(" Enum Property"));
    QStringList enumNames;
    enumNames << "Enum0" << "Enum1" << "Enum2";
    item->setAttribute(QLatin1String("enumNames"), enumNames);
    item->setValue(1);
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QtVariantPropertyManager::flagTypeId(),
                    QString::number(i++) + QLatin1String(" Flag Property"));
    QStringList flagNames;
    flagNames << "Flag0" << "Flag1" << "Flag2";
    item->setAttribute(QLatin1String("flagNames"), flagNames);
    item->setValue(5);
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::SizePolicy, QString::number(i++) + QLatin1String(" SizePolicy Property"));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Font, QString::number(i++) + QLatin1String(" Font Property"));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Cursor, QString::number(i++) + QLatin1String(" Cursor Property"));
    topItem->addSubProperty(item);
*/
    item = variantManager->addProperty(QVariant::Color, QString::number(i++) + QLatin1String(" Color Property"));
	item->setValue(Qt::blue);
    topItem->addSubProperty(item);

    variantFactory = new QtVariantEditorFactory();

    variantEditor = new QtTreePropertyBrowser();
    variantEditor->setFactoryForManager(variantManager, variantFactory);
    variantEditor->addProperty(topItem);
    variantEditor->setPropertiesWithoutValueMarked(true);
    variantEditor->setRootIsDecorated(true);

    connect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(valueChanged(QtProperty *, const QVariant &)));

    QDockWidget *dockWidget;

	dockWidget = new QDockWidget("Property Editor", this);
	dockWidget->setObjectName("Property Editor");
	dockWidget->setWidget(variantEditor);

	addDockWidget(Qt::RightDockWidgetArea,dockWidget);
}

void MainWindow::valueChanged(QtProperty *changedProperty, const QVariant &value)
{
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    QString shapeName = doc->currentShapeName();
    if (shapeName.isEmpty())
        return;

	if(changedProperty->propertyName().right(14) == "Color Property")
	{
		QColor color;

		color = value.value<QColor>();

		if (color == doc->shape(shapeName).color())
			return;

	    doc->undoStack()->push(new SetShapeColorCommand(doc, shapeName, color));
	}
}

void MainWindow::updateActions()
{
    Document *doc = currentDocument();
    m_undoGroup->setActiveStack(doc == 0 ? 0 : doc->undoStack());
	if(doc)
	{
		if(doc->fileName().isEmpty())
		{
			undoView->setEmptyLabel("[New File]");
		}
		else
		{
			QFileInfo fileInfo(doc->fileName());
		
			undoView->setEmptyLabel(QString("[Opened File] %1").arg(fileInfo.fileName()));
		}
	}
    QString shapeName = doc == 0 ? QString() : doc->currentShapeName();

    actionAddCircle->setEnabled(doc != 0);
    actionAddRectangle->setEnabled(doc != 0);
    actionAddTriangle->setEnabled(doc != 0);
    actionSave->setEnabled(doc != 0 && !doc->undoStack()->isClean());

    if (shapeName.isEmpty()) {
        actionRemoveShape->setEnabled(false);
    } else {
        Shape shape = doc->shape(shapeName);
        actionRemoveShape->setEnabled(true);
    }

    if (doc != 0) {
        int index = documentTabs->indexOf(doc);
        Q_ASSERT(index != -1);
        static const QIcon unsavedIcon(":/icons/red.png");
        documentTabs->setTabIcon(index, doc->undoStack()->isClean() ? QIcon() : unsavedIcon);
    }
}

void MainWindow::openDocument()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this,
                            tr("File error"),
                            tr("Failed to open\n%1").arg(fileName));
        return;
    }
    QTextStream stream(&file);

    Document *doc = new Document();
    if (!doc->load(stream)) {
        QMessageBox::warning(this,
                            tr("Parse error"),
                            tr("Failed to parse\n%1").arg(fileName));
        delete doc;
        return;
    }

    doc->setFileName(fileName);
    addDocument(doc);
}

QString MainWindow::fixedWindowTitle(const Document *doc) const
{
    QString title = doc->fileName();

    if (title.isEmpty())
        title = tr("Unnamed");
    else
        title = QFileInfo(title).fileName();

    QString result;

    for (int i = 0; ; ++i) {
        result = title;
        if (i > 0)
            result += QString::number(i);

        bool unique = true;
        for (int j = 0; j < documentTabs->count(); ++j) {
            const QWidget *widget = documentTabs->widget(j);
            if (widget == doc)
                continue;
            if (result == documentTabs->tabText(j)) {
                unique = false;
                break;
            }
        }

        if (unique)
            break;
    }

    return result;
}

void MainWindow::addDocument(Document *doc)
{
    if (documentTabs->indexOf(doc) != -1)
        return;
    m_undoGroup->addStack(doc->undoStack());
    documentTabs->addTab(doc, fixedWindowTitle(doc));
    connect(doc, SIGNAL(currentShapeChanged(QString)), this, SLOT(updateActions()));
    connect(doc->undoStack(), SIGNAL(indexChanged(int)), this, SLOT(updateActions()));
    connect(doc->undoStack(), SIGNAL(cleanChanged(bool)), this, SLOT(updateActions()));

    setCurrentDocument(doc);
}

void MainWindow::setCurrentDocument(Document *doc)
{
    documentTabs->setCurrentWidget(doc);
}

Document *MainWindow::currentDocument() const
{
    return qobject_cast<Document*>(documentTabs->currentWidget());
}

void MainWindow::removeDocument(Document *doc)
{
    int index = documentTabs->indexOf(doc);
    if (index == -1)
        return;

    documentTabs->removeTab(index);
    m_undoGroup->removeStack(doc->undoStack());
    disconnect(doc, SIGNAL(currentShapeChanged(QString)), this, SLOT(updateActions()));
    disconnect(doc->undoStack(), SIGNAL(indexChanged(int)), this, SLOT(updateActions()));
    disconnect(doc->undoStack(), SIGNAL(cleanChanged(bool)), this, SLOT(updateActions()));

    if (documentTabs->count() == 0) {
        newDocument();
        updateActions();
    }
}

void MainWindow::saveDocument()
{
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    for (;;) {
        QString fileName = doc->fileName();

        if (fileName.isEmpty())
            fileName = QFileDialog::getSaveFileName(this);
        if (fileName.isEmpty())
            break;

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this,
                                tr("File error"),
                                tr("Failed to open\n%1").arg(fileName));
            doc->setFileName(QString());
        } else {
            QTextStream stream(&file);
            doc->save(stream);
            doc->setFileName(fileName);

            int index = documentTabs->indexOf(doc);
            Q_ASSERT(index != -1);
            documentTabs->setTabText(index, fixedWindowTitle(doc));

            break;
        }
    }
}

void MainWindow::closeDocument(int index)
{
	Document *doc;

	if(index==-1)
	{
	    doc = currentDocument();
	}
	else
	{
	    doc = qobject_cast<Document*>(documentTabs->widget(index));
	}

	if (doc == 0)
        return;

    if (!doc->undoStack()->isClean()) {
        int button
            = QMessageBox::warning(this,
                            tr("Unsaved changes"),
                            tr("Would you like to save this document?"),
                            QMessageBox::Yes, QMessageBox::No);
        if (button == QMessageBox::Yes)
            saveDocument();
    }

    removeDocument(doc);
    delete doc;
}

void MainWindow::newDocument()
{
    addDocument(new Document());
}

static QColor randomColor()
{
    int r = (int) (3.0*(rand()/(RAND_MAX + 1.0)));
    switch (r) {
        case 0:
            return Qt::red;
        case 1:
            return Qt::green;
        default:
            break;
    }
    return Qt::blue;
}

static QRect randomRect(const QSize &s)
{
    QSize min = Shape::minSize;

    int left = (int) ((0.0 + s.width() - min.width())*(rand()/(RAND_MAX + 1.0)));
    int top = (int) ((0.0 + s.height() - min.height())*(rand()/(RAND_MAX + 1.0)));
    int width = (int) ((0.0 + s.width() - left - min.width())*(rand()/(RAND_MAX + 1.0))) + min.width();
    int height = (int) ((0.0 + s.height() - top - min.height())*(rand()/(RAND_MAX + 1.0))) + min.height();

    return QRect(left, top, width, height);
}

void MainWindow::addShape()
{
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    Shape::Type type;

    if (sender() == actionAddCircle)
        type = Shape::Circle;
    else if (sender() == actionAddRectangle)
        type = Shape::Rectangle;
    else if (sender() == actionAddTriangle)
        type = Shape::Triangle;
    else return;

	Shape newShape(type, Qt::blue, QRect(10,10,60,60));
    doc->undoStack()->push(new AddShapeCommand(doc, newShape));
}

void MainWindow::removeShape()
{
    Document *doc = currentDocument();
    if (doc == 0)
        return;

    QString shapeName = doc->currentShapeName();
    if (shapeName.isEmpty())
        return;

    doc->undoStack()->push(new RemoveShapeCommand(doc, shapeName));
}

void MainWindow::toggleClipping()
{
    Document *doc = currentDocument();
    if (doc == 0)
        return;

	doc->toggleClippingState();
}

void MainWindow::beginDebug()
{
    debugger->action(QScriptEngineDebugger::InterruptAction)->trigger();
    engine->evaluate("function foo()\n"
					 "{\n"
					 "  var j = -1;\n"
					 "  j += Math.random();\n"
					 "  j--;\n"
					 "}\n"
					 "\n"
					 "var i = 1\n"
		             "i++\n"
					 "foo()\n"
					 "i=5\n"
					 "i--;", "Script");
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Picto"), tr("Picto Workstation"));
}
