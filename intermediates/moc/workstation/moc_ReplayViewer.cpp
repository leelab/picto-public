/****************************************************************************
** Meta object code from reading C++ file 'ReplayViewer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/workstation/ReplayViewer/ReplayViewer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ReplayViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ReplayViewer_t {
    QByteArrayData data[56];
    char stringdata0[664];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ReplayViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ReplayViewer_t qt_meta_stringdata_ReplayViewer = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ReplayViewer"
QT_MOC_LITERAL(1, 13, 4), // "init"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 6), // "deinit"
QT_MOC_LITERAL(4, 26, 11), // "aboutToQuit"
QT_MOC_LITERAL(5, 38, 9), // "runNormal"
QT_MOC_LITERAL(6, 48, 8), // "runToEnd"
QT_MOC_LITERAL(7, 57, 4), // "play"
QT_MOC_LITERAL(8, 62, 5), // "pause"
QT_MOC_LITERAL(9, 68, 4), // "stop"
QT_MOC_LITERAL(10, 73, 10), // "spikeAdded"
QT_MOC_LITERAL(11, 84, 7), // "channel"
QT_MOC_LITERAL(12, 92, 4), // "unit"
QT_MOC_LITERAL(13, 97, 4), // "time"
QT_MOC_LITERAL(14, 102, 8), // "rewarded"
QT_MOC_LITERAL(15, 111, 8), // "quantity"
QT_MOC_LITERAL(16, 120, 12), // "openInNewTab"
QT_MOC_LITERAL(17, 133, 8), // "QWidget*"
QT_MOC_LITERAL(18, 142, 7), // "pWidget"
QT_MOC_LITERAL(19, 150, 8), // "closeTab"
QT_MOC_LITERAL(20, 159, 5), // "index"
QT_MOC_LITERAL(21, 165, 21), // "playbackStatusChanged"
QT_MOC_LITERAL(22, 187, 6), // "status"
QT_MOC_LITERAL(23, 194, 11), // "loadSession"
QT_MOC_LITERAL(24, 206, 10), // "updateTime"
QT_MOC_LITERAL(25, 217, 19), // "runSelectionChanged"
QT_MOC_LITERAL(26, 237, 21), // "analysisWidgetChanged"
QT_MOC_LITERAL(27, 259, 11), // "setUserType"
QT_MOC_LITERAL(28, 271, 18), // "setLFPRequirements"
QT_MOC_LITERAL(29, 290, 13), // "percentLoaded"
QT_MOC_LITERAL(30, 304, 7), // "percent"
QT_MOC_LITERAL(31, 312, 13), // "jumpRequested"
QT_MOC_LITERAL(32, 326, 16), // "userChoosingJump"
QT_MOC_LITERAL(33, 343, 8), // "starting"
QT_MOC_LITERAL(34, 352, 15), // "toggleRecording"
QT_MOC_LITERAL(35, 368, 16), // "restartRecording"
QT_MOC_LITERAL(36, 385, 13), // "saveRecording"
QT_MOC_LITERAL(37, 399, 13), // "setRecordTime"
QT_MOC_LITERAL(38, 413, 17), // "designRootChanged"
QT_MOC_LITERAL(39, 431, 12), // "preloadError"
QT_MOC_LITERAL(40, 444, 8), // "errorStr"
QT_MOC_LITERAL(41, 453, 10), // "runStarted"
QT_MOC_LITERAL(42, 464, 5), // "runId"
QT_MOC_LITERAL(43, 470, 16), // "finishedPlayback"
QT_MOC_LITERAL(44, 487, 9), // "loadError"
QT_MOC_LITERAL(45, 497, 8), // "errorMsg"
QT_MOC_LITERAL(46, 506, 16), // "sessionPreloaded"
QT_MOC_LITERAL(47, 523, 20), // "PreloadedSessionData"
QT_MOC_LITERAL(48, 544, 11), // "sessionData"
QT_MOC_LITERAL(49, 556, 11), // "taskChanged"
QT_MOC_LITERAL(50, 568, 7), // "newTask"
QT_MOC_LITERAL(51, 576, 22), // "selectedChannelChanged"
QT_MOC_LITERAL(52, 599, 19), // "selectedUnitChanged"
QT_MOC_LITERAL(53, 619, 13), // "sessionLoaded"
QT_MOC_LITERAL(54, 633, 17), // "LoadedSessionData"
QT_MOC_LITERAL(55, 651, 12) // "checkClicked"

    },
    "ReplayViewer\0init\0\0deinit\0aboutToQuit\0"
    "runNormal\0runToEnd\0play\0pause\0stop\0"
    "spikeAdded\0channel\0unit\0time\0rewarded\0"
    "quantity\0openInNewTab\0QWidget*\0pWidget\0"
    "closeTab\0index\0playbackStatusChanged\0"
    "status\0loadSession\0updateTime\0"
    "runSelectionChanged\0analysisWidgetChanged\0"
    "setUserType\0setLFPRequirements\0"
    "percentLoaded\0percent\0jumpRequested\0"
    "userChoosingJump\0starting\0toggleRecording\0"
    "restartRecording\0saveRecording\0"
    "setRecordTime\0designRootChanged\0"
    "preloadError\0errorStr\0runStarted\0runId\0"
    "finishedPlayback\0loadError\0errorMsg\0"
    "sessionPreloaded\0PreloadedSessionData\0"
    "sessionData\0taskChanged\0newTask\0"
    "selectedChannelChanged\0selectedUnitChanged\0"
    "sessionLoaded\0LoadedSessionData\0"
    "checkClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ReplayViewer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      37,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  199,    2, 0x0a /* Public */,
       3,    0,  200,    2, 0x0a /* Public */,
       4,    0,  201,    2, 0x0a /* Public */,
       5,    0,  202,    2, 0x0a /* Public */,
       6,    0,  203,    2, 0x0a /* Public */,
       7,    0,  204,    2, 0x0a /* Public */,
       8,    0,  205,    2, 0x0a /* Public */,
       9,    0,  206,    2, 0x0a /* Public */,
      10,    3,  207,    2, 0x0a /* Public */,
      14,    1,  214,    2, 0x0a /* Public */,
      16,    1,  217,    2, 0x0a /* Public */,
      19,    1,  220,    2, 0x0a /* Public */,
      21,    1,  223,    2, 0x08 /* Private */,
      23,    0,  226,    2, 0x08 /* Private */,
      24,    1,  227,    2, 0x08 /* Private */,
      25,    0,  230,    2, 0x08 /* Private */,
      26,    0,  231,    2, 0x08 /* Private */,
      27,    1,  232,    2, 0x08 /* Private */,
      28,    1,  235,    2, 0x08 /* Private */,
      29,    1,  238,    2, 0x08 /* Private */,
      31,    1,  241,    2, 0x08 /* Private */,
      32,    1,  244,    2, 0x08 /* Private */,
      34,    0,  247,    2, 0x08 /* Private */,
      35,    0,  248,    2, 0x08 /* Private */,
      36,    0,  249,    2, 0x08 /* Private */,
      37,    1,  250,    2, 0x08 /* Private */,
      38,    0,  253,    2, 0x08 /* Private */,
      39,    1,  254,    2, 0x08 /* Private */,
      41,    1,  257,    2, 0x08 /* Private */,
      43,    0,  260,    2, 0x08 /* Private */,
      44,    1,  261,    2, 0x08 /* Private */,
      46,    1,  264,    2, 0x08 /* Private */,
      49,    1,  267,    2, 0x08 /* Private */,
      51,    0,  270,    2, 0x08 /* Private */,
      52,    0,  271,    2, 0x08 /* Private */,
      53,    1,  272,    2, 0x08 /* Private */,
      55,    1,  275,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Double,   11,   12,   13,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, QMetaType::Int,   20,
    QMetaType::Void, QMetaType::Int,   22,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   20,
    QMetaType::Void, QMetaType::Int,   20,
    QMetaType::Void, QMetaType::Double,   30,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Bool,   33,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   40,
    QMetaType::Void, QMetaType::QUuid,   42,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   45,
    QMetaType::Void, 0x80000000 | 47,   48,
    QMetaType::Void, QMetaType::QString,   50,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 54,   48,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void ReplayViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ReplayViewer *_t = static_cast<ReplayViewer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->init(); break;
        case 1: _t->deinit(); break;
        case 2: { bool _r = _t->aboutToQuit();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: _t->runNormal(); break;
        case 4: _t->runToEnd(); break;
        case 5: { bool _r = _t->play();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: _t->pause(); break;
        case 7: _t->stop(); break;
        case 8: _t->spikeAdded((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 9: _t->rewarded((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->openInNewTab((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 11: _t->closeTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->playbackStatusChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->loadSession(); break;
        case 14: _t->updateTime((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 15: _t->runSelectionChanged(); break;
        case 16: _t->analysisWidgetChanged(); break;
        case 17: _t->setUserType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->setLFPRequirements((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->percentLoaded((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 20: _t->jumpRequested((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 21: _t->userChoosingJump((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 22: _t->toggleRecording(); break;
        case 23: _t->restartRecording(); break;
        case 24: _t->saveRecording(); break;
        case 25: _t->setRecordTime((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 26: _t->designRootChanged(); break;
        case 27: _t->preloadError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 28: _t->runStarted((*reinterpret_cast< QUuid(*)>(_a[1]))); break;
        case 29: _t->finishedPlayback(); break;
        case 30: _t->loadError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 31: _t->sessionPreloaded((*reinterpret_cast< PreloadedSessionData(*)>(_a[1]))); break;
        case 32: _t->taskChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 33: _t->selectedChannelChanged(); break;
        case 34: _t->selectedUnitChanged(); break;
        case 35: _t->sessionLoaded((*reinterpret_cast< LoadedSessionData(*)>(_a[1]))); break;
        case 36: _t->checkClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        case 31:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< PreloadedSessionData >(); break;
            }
            break;
        case 35:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< LoadedSessionData >(); break;
            }
            break;
        }
    }
}

const QMetaObject ReplayViewer::staticMetaObject = {
    { &Viewer::staticMetaObject, qt_meta_stringdata_ReplayViewer.data,
      qt_meta_data_ReplayViewer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ReplayViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ReplayViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ReplayViewer.stringdata0))
        return static_cast<void*>(const_cast< ReplayViewer*>(this));
    return Viewer::qt_metacast(_clname);
}

int ReplayViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Viewer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 37)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 37;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 37)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 37;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
