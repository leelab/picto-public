/****************************************************************************
** Meta object code from reading C++ file 'TaskRunViewer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/workstation/RemoteViewer/TaskRunViewer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TaskRunViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_TaskRunViewer_t {
    QByteArrayData data[10];
    char stringdata0[121];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TaskRunViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TaskRunViewer_t qt_meta_stringdata_TaskRunViewer = {
    {
QT_MOC_LITERAL(0, 0, 13), // "TaskRunViewer"
QT_MOC_LITERAL(1, 14, 18), // "taskRunDataChanged"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 5), // "runId"
QT_MOC_LITERAL(4, 40, 12), // "actTriggered"
QT_MOC_LITERAL(5, 53, 15), // "cancelTriggered"
QT_MOC_LITERAL(6, 69, 25), // "availableRunsIndexChanged"
QT_MOC_LITERAL(7, 95, 5), // "index"
QT_MOC_LITERAL(8, 101, 11), // "saveToggled"
QT_MOC_LITERAL(9, 113, 7) // "checked"

    },
    "TaskRunViewer\0taskRunDataChanged\0\0"
    "runId\0actTriggered\0cancelTriggered\0"
    "availableRunsIndexChanged\0index\0"
    "saveToggled\0checked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TaskRunViewer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   42,    2, 0x08 /* Private */,
       5,    0,   43,    2, 0x08 /* Private */,
       6,    1,   44,    2, 0x08 /* Private */,
       8,    1,   47,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::ULongLong,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Bool,    9,

       0        // eod
};

void TaskRunViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TaskRunViewer *_t = static_cast<TaskRunViewer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->taskRunDataChanged((*reinterpret_cast< qulonglong(*)>(_a[1]))); break;
        case 1: _t->actTriggered(); break;
        case 2: _t->cancelTriggered(); break;
        case 3: _t->availableRunsIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->saveToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (TaskRunViewer::*_t)(qulonglong );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TaskRunViewer::taskRunDataChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject TaskRunViewer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TaskRunViewer.data,
      qt_meta_data_TaskRunViewer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *TaskRunViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TaskRunViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_TaskRunViewer.stringdata0))
        return static_cast<void*>(const_cast< TaskRunViewer*>(this));
    return QWidget::qt_metacast(_clname);
}

int TaskRunViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void TaskRunViewer::taskRunDataChanged(qulonglong _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
