/****************************************************************************
** Meta object code from reading C++ file 'TestViewer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/workstation/TestViewer/TestViewer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TestViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_TestViewer_t {
    QByteArrayData data[30];
    char stringdata0[336];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TestViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TestViewer_t qt_meta_stringdata_TestViewer = {
    {
QT_MOC_LITERAL(0, 0, 10), // "TestViewer"
QT_MOC_LITERAL(1, 11, 4), // "init"
QT_MOC_LITERAL(2, 16, 0), // ""
QT_MOC_LITERAL(3, 17, 6), // "deinit"
QT_MOC_LITERAL(4, 24, 11), // "aboutToQuit"
QT_MOC_LITERAL(5, 36, 20), // "LoadPropValsFromFile"
QT_MOC_LITERAL(6, 57, 10), // "spikeAdded"
QT_MOC_LITERAL(7, 68, 4), // "time"
QT_MOC_LITERAL(8, 73, 7), // "channel"
QT_MOC_LITERAL(9, 81, 4), // "unit"
QT_MOC_LITERAL(10, 86, 8), // "waveform"
QT_MOC_LITERAL(11, 95, 8), // "rewarded"
QT_MOC_LITERAL(12, 104, 8), // "quantity"
QT_MOC_LITERAL(13, 113, 13), // "playTriggered"
QT_MOC_LITERAL(14, 127, 7), // "running"
QT_MOC_LITERAL(15, 135, 6), // "paused"
QT_MOC_LITERAL(16, 142, 7), // "stopped"
QT_MOC_LITERAL(17, 150, 20), // "taskListIndexChanged"
QT_MOC_LITERAL(18, 171, 5), // "index"
QT_MOC_LITERAL(19, 177, 21), // "operatorClickDetected"
QT_MOC_LITERAL(20, 199, 3), // "pos"
QT_MOC_LITERAL(21, 203, 11), // "setUserType"
QT_MOC_LITERAL(22, 215, 10), // "runStarted"
QT_MOC_LITERAL(23, 226, 5), // "runId"
QT_MOC_LITERAL(24, 232, 15), // "toggleRecording"
QT_MOC_LITERAL(25, 248, 16), // "restartRecording"
QT_MOC_LITERAL(26, 265, 13), // "saveRecording"
QT_MOC_LITERAL(27, 279, 13), // "setRecordTime"
QT_MOC_LITERAL(28, 293, 22), // "selectedChannelChanged"
QT_MOC_LITERAL(29, 316, 19) // "selectedUnitChanged"

    },
    "TestViewer\0init\0\0deinit\0aboutToQuit\0"
    "LoadPropValsFromFile\0spikeAdded\0time\0"
    "channel\0unit\0waveform\0rewarded\0quantity\0"
    "playTriggered\0running\0paused\0stopped\0"
    "taskListIndexChanged\0index\0"
    "operatorClickDetected\0pos\0setUserType\0"
    "runStarted\0runId\0toggleRecording\0"
    "restartRecording\0saveRecording\0"
    "setRecordTime\0selectedChannelChanged\0"
    "selectedUnitChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TestViewer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  114,    2, 0x0a /* Public */,
       3,    0,  115,    2, 0x0a /* Public */,
       4,    0,  116,    2, 0x0a /* Public */,
       5,    0,  117,    2, 0x0a /* Public */,
       6,    4,  118,    2, 0x0a /* Public */,
      11,    1,  127,    2, 0x0a /* Public */,
      13,    0,  130,    2, 0x08 /* Private */,
      14,    0,  131,    2, 0x08 /* Private */,
      15,    0,  132,    2, 0x08 /* Private */,
      16,    0,  133,    2, 0x08 /* Private */,
      17,    1,  134,    2, 0x08 /* Private */,
      19,    1,  137,    2, 0x08 /* Private */,
      21,    1,  140,    2, 0x08 /* Private */,
      22,    1,  143,    2, 0x08 /* Private */,
      24,    0,  146,    2, 0x08 /* Private */,
      25,    0,  147,    2, 0x08 /* Private */,
      26,    0,  148,    2, 0x08 /* Private */,
      27,    1,  149,    2, 0x08 /* Private */,
      28,    1,  152,    2, 0x08 /* Private */,
      29,    1,  155,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double, QMetaType::Int, QMetaType::Int, QMetaType::QVariantList,    7,    8,    9,   10,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   18,
    QMetaType::Void, QMetaType::QPoint,   20,
    QMetaType::Void, QMetaType::Int,   18,
    QMetaType::Void, QMetaType::QUuid,   23,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    7,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    9,

       0        // eod
};

void TestViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TestViewer *_t = static_cast<TestViewer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->init(); break;
        case 1: _t->deinit(); break;
        case 2: { bool _r = _t->aboutToQuit();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: _t->LoadPropValsFromFile(); break;
        case 4: _t->spikeAdded((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< QVariantList(*)>(_a[4]))); break;
        case 5: _t->rewarded((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->playTriggered(); break;
        case 7: _t->running(); break;
        case 8: _t->paused(); break;
        case 9: _t->stopped(); break;
        case 10: _t->taskListIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->operatorClickDetected((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 12: _t->setUserType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->runStarted((*reinterpret_cast< QUuid(*)>(_a[1]))); break;
        case 14: _t->toggleRecording(); break;
        case 15: _t->restartRecording(); break;
        case 16: _t->saveRecording(); break;
        case 17: _t->setRecordTime((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 18: _t->selectedChannelChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->selectedUnitChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject TestViewer::staticMetaObject = {
    { &Viewer::staticMetaObject, qt_meta_stringdata_TestViewer.data,
      qt_meta_data_TestViewer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *TestViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TestViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_TestViewer.stringdata0))
        return static_cast<void*>(const_cast< TestViewer*>(this));
    return Viewer::qt_metacast(_clname);
}

int TestViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Viewer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 20;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
