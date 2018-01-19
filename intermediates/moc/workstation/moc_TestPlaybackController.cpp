/****************************************************************************
** Meta object code from reading C++ file 'TestPlaybackController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/workstation/TestViewer/TestPlaybackController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TestPlaybackController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_TestPlaybackController_t {
    QByteArrayData data[8];
    char stringdata0[63];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TestPlaybackController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TestPlaybackController_t qt_meta_stringdata_TestPlaybackController = {
    {
QT_MOC_LITERAL(0, 0, 22), // "TestPlaybackController"
QT_MOC_LITERAL(1, 23, 7), // "running"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 6), // "paused"
QT_MOC_LITERAL(4, 39, 7), // "stopped"
QT_MOC_LITERAL(5, 47, 4), // "play"
QT_MOC_LITERAL(6, 52, 5), // "pause"
QT_MOC_LITERAL(7, 58, 4) // "stop"

    },
    "TestPlaybackController\0running\0\0paused\0"
    "stopped\0play\0pause\0stop"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TestPlaybackController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    0,   45,    2, 0x06 /* Public */,
       4,    0,   46,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   47,    2, 0x0a /* Public */,
       6,    0,   48,    2, 0x0a /* Public */,
       7,    0,   49,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void TestPlaybackController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TestPlaybackController *_t = static_cast<TestPlaybackController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->running(); break;
        case 1: _t->paused(); break;
        case 2: _t->stopped(); break;
        case 3: _t->play(); break;
        case 4: _t->pause(); break;
        case 5: _t->stop(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (TestPlaybackController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TestPlaybackController::running)) {
                *result = 0;
            }
        }
        {
            typedef void (TestPlaybackController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TestPlaybackController::paused)) {
                *result = 1;
            }
        }
        {
            typedef void (TestPlaybackController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TestPlaybackController::stopped)) {
                *result = 2;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject TestPlaybackController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_TestPlaybackController.data,
      qt_meta_data_TestPlaybackController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *TestPlaybackController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TestPlaybackController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_TestPlaybackController.stringdata0))
        return static_cast<void*>(const_cast< TestPlaybackController*>(this));
    return QObject::qt_metacast(_clname);
}

int TestPlaybackController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void TestPlaybackController::running()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void TestPlaybackController::paused()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void TestPlaybackController::stopped()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
