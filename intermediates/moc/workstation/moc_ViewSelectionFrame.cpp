/****************************************************************************
** Meta object code from reading C++ file 'ViewSelectionFrame.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/workstation/DataViewer/ViewSelectionFrame.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ViewSelectionFrame.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_lblEdit_t {
    QByteArrayData data[1];
    char stringdata0[8];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_lblEdit_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_lblEdit_t qt_meta_stringdata_lblEdit = {
    {
QT_MOC_LITERAL(0, 0, 7) // "lblEdit"

    },
    "lblEdit"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_lblEdit[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void lblEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject lblEdit::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_lblEdit.data,
      qt_meta_data_lblEdit,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *lblEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *lblEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_lblEdit.stringdata0))
        return static_cast<void*>(const_cast< lblEdit*>(this));
    return QWidget::qt_metacast(_clname);
}

int lblEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_ViewSelectionFrame_t {
    QByteArrayData data[44];
    char stringdata0[741];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ViewSelectionFrame_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ViewSelectionFrame_t qt_meta_stringdata_ViewSelectionFrame = {
    {
QT_MOC_LITERAL(0, 0, 18), // "ViewSelectionFrame"
QT_MOC_LITERAL(1, 19, 13), // "widgetRemoved"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 8), // "QWidget*"
QT_MOC_LITERAL(4, 43, 7), // "pWidget"
QT_MOC_LITERAL(5, 51, 11), // "widgetAdded"
QT_MOC_LITERAL(6, 63, 1), // "x"
QT_MOC_LITERAL(7, 65, 1), // "y"
QT_MOC_LITERAL(8, 67, 22), // "DataViewSize::ViewSize"
QT_MOC_LITERAL(9, 90, 4), // "size"
QT_MOC_LITERAL(10, 95, 17), // "returnPlotHandler"
QT_MOC_LITERAL(11, 113, 35), // "QSharedPointer<OperatorPlotHa..."
QT_MOC_LITERAL(12, 149, 7), // "handler"
QT_MOC_LITERAL(13, 157, 8), // "plotPath"
QT_MOC_LITERAL(14, 166, 28), // "managerConnectionEstablished"
QT_MOC_LITERAL(15, 195, 9), // "connected"
QT_MOC_LITERAL(16, 205, 12), // "openInNewTab"
QT_MOC_LITERAL(17, 218, 21), // "updateWidgetContainer"
QT_MOC_LITERAL(18, 240, 21), // "removeWidgetContainer"
QT_MOC_LITERAL(19, 262, 18), // "addWidgetContainer"
QT_MOC_LITERAL(20, 281, 23), // "notifyAnalysisSelection"
QT_MOC_LITERAL(21, 305, 4), // "name"
QT_MOC_LITERAL(22, 310, 8), // "selected"
QT_MOC_LITERAL(23, 319, 5), // "local"
QT_MOC_LITERAL(24, 325, 22), // "clearAnalysisSelection"
QT_MOC_LITERAL(25, 348, 17), // "clearPlotHandlers"
QT_MOC_LITERAL(26, 366, 13), // "widgetDropped"
QT_MOC_LITERAL(27, 380, 6), // "offset"
QT_MOC_LITERAL(28, 387, 13), // "widgetDragged"
QT_MOC_LITERAL(29, 401, 24), // "selectedPlotIndexChanged"
QT_MOC_LITERAL(30, 426, 13), // "selectedIndex"
QT_MOC_LITERAL(31, 440, 24), // "selectedSizeIndexChanged"
QT_MOC_LITERAL(32, 465, 12), // "checkClicked"
QT_MOC_LITERAL(33, 478, 20), // "plotHandlerRequested"
QT_MOC_LITERAL(34, 499, 16), // "cachePlotHandler"
QT_MOC_LITERAL(35, 516, 29), // "selectedLevelIndexChangedNeur"
QT_MOC_LITERAL(36, 546, 28), // "selectedLevelIndexChangedHor"
QT_MOC_LITERAL(37, 575, 27), // "selectedLevelIndexChangedAl"
QT_MOC_LITERAL(38, 603, 27), // "selectedLevelIndexChangedSc"
QT_MOC_LITERAL(39, 631, 28), // "selectedLevelIndexChangedVal"
QT_MOC_LITERAL(40, 660, 29), // "selectedLevelIndexChangedMark"
QT_MOC_LITERAL(41, 690, 23), // "selectedPlotTypeChanged"
QT_MOC_LITERAL(42, 714, 12), // "readSettings"
QT_MOC_LITERAL(43, 727, 13) // "writeSettings"

    },
    "ViewSelectionFrame\0widgetRemoved\0\0"
    "QWidget*\0pWidget\0widgetAdded\0x\0y\0"
    "DataViewSize::ViewSize\0size\0"
    "returnPlotHandler\0QSharedPointer<OperatorPlotHandler>\0"
    "handler\0plotPath\0managerConnectionEstablished\0"
    "connected\0openInNewTab\0updateWidgetContainer\0"
    "removeWidgetContainer\0addWidgetContainer\0"
    "notifyAnalysisSelection\0name\0selected\0"
    "local\0clearAnalysisSelection\0"
    "clearPlotHandlers\0widgetDropped\0offset\0"
    "widgetDragged\0selectedPlotIndexChanged\0"
    "selectedIndex\0selectedSizeIndexChanged\0"
    "checkClicked\0plotHandlerRequested\0"
    "cachePlotHandler\0selectedLevelIndexChangedNeur\0"
    "selectedLevelIndexChangedHor\0"
    "selectedLevelIndexChangedAl\0"
    "selectedLevelIndexChangedSc\0"
    "selectedLevelIndexChangedVal\0"
    "selectedLevelIndexChangedMark\0"
    "selectedPlotTypeChanged\0readSettings\0"
    "writeSettings"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ViewSelectionFrame[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      27,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  149,    2, 0x06 /* Public */,
       5,    4,  152,    2, 0x06 /* Public */,
      10,    2,  161,    2, 0x06 /* Public */,
      14,    1,  166,    2, 0x06 /* Public */,
      16,    1,  169,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      17,    1,  172,    2, 0x0a /* Public */,
      18,    1,  175,    2, 0x0a /* Public */,
      19,    1,  178,    2, 0x0a /* Public */,
      20,    3,  181,    2, 0x0a /* Public */,
      24,    1,  188,    2, 0x0a /* Public */,
      25,    0,  191,    2, 0x0a /* Public */,
      26,    1,  192,    2, 0x0a /* Public */,
      28,    1,  195,    2, 0x0a /* Public */,
      29,    1,  198,    2, 0x08 /* Private */,
      31,    1,  201,    2, 0x08 /* Private */,
      32,    1,  204,    2, 0x08 /* Private */,
      33,    1,  207,    2, 0x08 /* Private */,
      34,    2,  210,    2, 0x08 /* Private */,
      35,    0,  215,    2, 0x08 /* Private */,
      36,    0,  216,    2, 0x08 /* Private */,
      37,    0,  217,    2, 0x08 /* Private */,
      38,    0,  218,    2, 0x08 /* Private */,
      39,    0,  219,    2, 0x08 /* Private */,
      40,    0,  220,    2, 0x08 /* Private */,
      41,    1,  221,    2, 0x08 /* Private */,
      42,    0,  224,    2, 0x08 /* Private */,
      43,    0,  225,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Int, 0x80000000 | 8,    4,    6,    7,    9,
    QMetaType::Void, 0x80000000 | 11, QMetaType::QString,   12,   13,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,   21,   22,   23,
    QMetaType::Void, QMetaType::Bool,   23,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   27,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Int,   30,
    QMetaType::Void, QMetaType::Int,   30,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void, 0x80000000 | 11, QMetaType::QString,   12,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ViewSelectionFrame::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ViewSelectionFrame *_t = static_cast<ViewSelectionFrame *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->widgetRemoved((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 1: _t->widgetAdded((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< DataViewSize::ViewSize(*)>(_a[4]))); break;
        case 2: _t->returnPlotHandler((*reinterpret_cast< QSharedPointer<OperatorPlotHandler>(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->managerConnectionEstablished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->openInNewTab((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 5: _t->updateWidgetContainer((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 6: _t->removeWidgetContainer((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 7: _t->addWidgetContainer((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 8: _t->notifyAnalysisSelection((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 9: _t->clearAnalysisSelection((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->clearPlotHandlers(); break;
        case 11: _t->widgetDropped((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 12: _t->widgetDragged((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 13: _t->selectedPlotIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->selectedSizeIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->checkClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: _t->plotHandlerRequested((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: _t->cachePlotHandler((*reinterpret_cast< QSharedPointer<OperatorPlotHandler>(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 18: _t->selectedLevelIndexChangedNeur(); break;
        case 19: _t->selectedLevelIndexChangedHor(); break;
        case 20: _t->selectedLevelIndexChangedAl(); break;
        case 21: _t->selectedLevelIndexChangedSc(); break;
        case 22: _t->selectedLevelIndexChangedVal(); break;
        case 23: _t->selectedLevelIndexChangedMark(); break;
        case 24: _t->selectedPlotTypeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 25: _t->readSettings(); break;
        case 26: _t->writeSettings(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<OperatorPlotHandler> >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        case 17:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<OperatorPlotHandler> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ViewSelectionFrame::*_t)(QWidget * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewSelectionFrame::widgetRemoved)) {
                *result = 0;
            }
        }
        {
            typedef void (ViewSelectionFrame::*_t)(QWidget * , int , int , DataViewSize::ViewSize );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewSelectionFrame::widgetAdded)) {
                *result = 1;
            }
        }
        {
            typedef void (ViewSelectionFrame::*_t)(QSharedPointer<OperatorPlotHandler> , const QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewSelectionFrame::returnPlotHandler)) {
                *result = 2;
            }
        }
        {
            typedef void (ViewSelectionFrame::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewSelectionFrame::managerConnectionEstablished)) {
                *result = 3;
            }
        }
        {
            typedef void (ViewSelectionFrame::*_t)(QWidget * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ViewSelectionFrame::openInNewTab)) {
                *result = 4;
            }
        }
    }
}

const QMetaObject ViewSelectionFrame::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_ViewSelectionFrame.data,
      qt_meta_data_ViewSelectionFrame,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ViewSelectionFrame::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ViewSelectionFrame::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ViewSelectionFrame.stringdata0))
        return static_cast<void*>(const_cast< ViewSelectionFrame*>(this));
    return QFrame::qt_metacast(_clname);
}

int ViewSelectionFrame::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    }
    return _id;
}

// SIGNAL 0
void ViewSelectionFrame::widgetRemoved(QWidget * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ViewSelectionFrame::widgetAdded(QWidget * _t1, int _t2, int _t3, DataViewSize::ViewSize _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ViewSelectionFrame::returnPlotHandler(QSharedPointer<OperatorPlotHandler> _t1, const QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ViewSelectionFrame::managerConnectionEstablished(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ViewSelectionFrame::openInNewTab(QWidget * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
