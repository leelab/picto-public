/****************************************************************************
** Meta object code from reading C++ file 'AnalysisOptionWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/AnalysisOptionWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AnalysisOptionWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__AnalysisOptionWidget_t {
    QByteArrayData data[18];
    char stringdata0[291];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__AnalysisOptionWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__AnalysisOptionWidget_t qt_meta_stringdata_Picto__AnalysisOptionWidget = {
    {
QT_MOC_LITERAL(0, 0, 27), // "Picto::AnalysisOptionWidget"
QT_MOC_LITERAL(1, 28, 18), // "updateAnalysisList"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 14), // "updateTaskList"
QT_MOC_LITERAL(4, 63, 11), // "updateLists"
QT_MOC_LITERAL(5, 75, 8), // "bQuietly"
QT_MOC_LITERAL(6, 84, 11), // "clearAssets"
QT_MOC_LITERAL(7, 96, 11), // "addAnalysis"
QT_MOC_LITERAL(8, 108, 22), // "deleteSelectedAnalysis"
QT_MOC_LITERAL(9, 131, 20), // "selectedIndexChanged"
QT_MOC_LITERAL(10, 152, 13), // "selectedIndex"
QT_MOC_LITERAL(11, 166, 23), // "selectedItemTextChanged"
QT_MOC_LITERAL(12, 190, 16), // "selectedItemText"
QT_MOC_LITERAL(13, 207, 24), // "selectedTaskIndexChanged"
QT_MOC_LITERAL(14, 232, 23), // "selectedTaskTextChanged"
QT_MOC_LITERAL(15, 256, 7), // "addTask"
QT_MOC_LITERAL(16, 264, 8), // "editTask"
QT_MOC_LITERAL(17, 273, 17) // "deleteCurrentTask"

    },
    "Picto::AnalysisOptionWidget\0"
    "updateAnalysisList\0\0updateTaskList\0"
    "updateLists\0bQuietly\0clearAssets\0"
    "addAnalysis\0deleteSelectedAnalysis\0"
    "selectedIndexChanged\0selectedIndex\0"
    "selectedItemTextChanged\0selectedItemText\0"
    "selectedTaskIndexChanged\0"
    "selectedTaskTextChanged\0addTask\0"
    "editTask\0deleteCurrentTask"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__AnalysisOptionWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x0a /* Public */,
       3,    0,   80,    2, 0x0a /* Public */,
       4,    1,   81,    2, 0x0a /* Public */,
       6,    0,   84,    2, 0x0a /* Public */,
       7,    0,   85,    2, 0x08 /* Private */,
       8,    0,   86,    2, 0x08 /* Private */,
       9,    1,   87,    2, 0x08 /* Private */,
      11,    1,   90,    2, 0x08 /* Private */,
      13,    1,   93,    2, 0x08 /* Private */,
      14,    1,   96,    2, 0x08 /* Private */,
      15,    0,   99,    2, 0x08 /* Private */,
      16,    0,  100,    2, 0x08 /* Private */,
      17,    0,  101,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Picto::AnalysisOptionWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AnalysisOptionWidget *_t = static_cast<AnalysisOptionWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateAnalysisList(); break;
        case 1: _t->updateTaskList(); break;
        case 2: _t->updateLists((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->clearAssets(); break;
        case 4: _t->addAnalysis(); break;
        case 5: _t->deleteSelectedAnalysis(); break;
        case 6: _t->selectedIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->selectedItemTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->selectedTaskIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->selectedTaskTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->addTask(); break;
        case 11: _t->editTask(); break;
        case 12: _t->deleteCurrentTask(); break;
        default: ;
        }
    }
}

const QMetaObject Picto::AnalysisOptionWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Picto__AnalysisOptionWidget.data,
      qt_meta_data_Picto__AnalysisOptionWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::AnalysisOptionWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::AnalysisOptionWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__AnalysisOptionWidget.stringdata0))
        return static_cast<void*>(const_cast< AnalysisOptionWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int Picto::AnalysisOptionWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
