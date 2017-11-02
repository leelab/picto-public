/****************************************************************************
** Meta object code from reading C++ file 'PlaybackController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/workstation/ReplayViewer/PlaybackController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlaybackController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PlaybackController_t {
    QByteArrayData data[49];
    char stringdata0[551];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PlaybackController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PlaybackController_t qt_meta_stringdata_PlaybackController = {
    {
QT_MOC_LITERAL(0, 0, 18), // "PlaybackController"
QT_MOC_LITERAL(1, 19, 16), // "sessionPreloaded"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 20), // "PreloadedSessionData"
QT_MOC_LITERAL(4, 58, 11), // "sessionData"
QT_MOC_LITERAL(5, 70, 20), // "sessionPreloadFailed"
QT_MOC_LITERAL(6, 91, 5), // "error"
QT_MOC_LITERAL(7, 97, 11), // "timeChanged"
QT_MOC_LITERAL(8, 109, 4), // "time"
QT_MOC_LITERAL(9, 114, 7), // "loading"
QT_MOC_LITERAL(10, 122, 9), // "isLoading"
QT_MOC_LITERAL(11, 132, 13), // "percentLoaded"
QT_MOC_LITERAL(12, 146, 7), // "percent"
QT_MOC_LITERAL(13, 154, 17), // "designRootChanged"
QT_MOC_LITERAL(14, 172, 13), // "statusChanged"
QT_MOC_LITERAL(15, 186, 6), // "status"
QT_MOC_LITERAL(16, 193, 16), // "finishedPlayback"
QT_MOC_LITERAL(17, 210, 9), // "loadError"
QT_MOC_LITERAL(18, 220, 3), // "msg"
QT_MOC_LITERAL(19, 224, 11), // "taskChanged"
QT_MOC_LITERAL(20, 236, 8), // "taskName"
QT_MOC_LITERAL(21, 245, 8), // "rewarded"
QT_MOC_LITERAL(22, 254, 8), // "quantity"
QT_MOC_LITERAL(23, 263, 12), // "alignPlotSig"
QT_MOC_LITERAL(24, 276, 7), // "alignID"
QT_MOC_LITERAL(25, 284, 13), // "sessionLoaded"
QT_MOC_LITERAL(26, 298, 17), // "LoadedSessionData"
QT_MOC_LITERAL(27, 316, 13), // "eventAddedSig"
QT_MOC_LITERAL(28, 330, 7), // "eventID"
QT_MOC_LITERAL(29, 338, 18), // "scriptContAddedSig"
QT_MOC_LITERAL(30, 357, 8), // "scriptID"
QT_MOC_LITERAL(31, 366, 4), // "stop"
QT_MOC_LITERAL(32, 371, 10), // "jumpToTime"
QT_MOC_LITERAL(33, 382, 11), // "setRunSpeed"
QT_MOC_LITERAL(34, 394, 5), // "value"
QT_MOC_LITERAL(35, 400, 17), // "setUserToOperator"
QT_MOC_LITERAL(36, 418, 16), // "setUserToSubject"
QT_MOC_LITERAL(37, 435, 13), // "enableLFPLoad"
QT_MOC_LITERAL(38, 449, 6), // "enable"
QT_MOC_LITERAL(39, 456, 10), // "selectFile"
QT_MOC_LITERAL(40, 467, 8), // "filePath"
QT_MOC_LITERAL(41, 476, 9), // "selectRun"
QT_MOC_LITERAL(42, 486, 5), // "index"
QT_MOC_LITERAL(43, 492, 12), // "newRunLength"
QT_MOC_LITERAL(44, 505, 6), // "length"
QT_MOC_LITERAL(45, 512, 11), // "setCurrTime"
QT_MOC_LITERAL(46, 524, 13), // "playbackEnded"
QT_MOC_LITERAL(47, 538, 5), // "setup"
QT_MOC_LITERAL(48, 544, 6) // "update"

    },
    "PlaybackController\0sessionPreloaded\0"
    "\0PreloadedSessionData\0sessionData\0"
    "sessionPreloadFailed\0error\0timeChanged\0"
    "time\0loading\0isLoading\0percentLoaded\0"
    "percent\0designRootChanged\0statusChanged\0"
    "status\0finishedPlayback\0loadError\0msg\0"
    "taskChanged\0taskName\0rewarded\0quantity\0"
    "alignPlotSig\0alignID\0sessionLoaded\0"
    "LoadedSessionData\0eventAddedSig\0eventID\0"
    "scriptContAddedSig\0scriptID\0stop\0"
    "jumpToTime\0setRunSpeed\0value\0"
    "setUserToOperator\0setUserToSubject\0"
    "enableLFPLoad\0enable\0selectFile\0"
    "filePath\0selectRun\0index\0newRunLength\0"
    "length\0setCurrTime\0playbackEnded\0setup\0"
    "update"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlaybackController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      28,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      15,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  154,    2, 0x06 /* Public */,
       5,    1,  157,    2, 0x06 /* Public */,
       7,    1,  160,    2, 0x06 /* Public */,
       9,    1,  163,    2, 0x06 /* Public */,
      11,    1,  166,    2, 0x06 /* Public */,
      13,    0,  169,    2, 0x06 /* Public */,
      14,    1,  170,    2, 0x06 /* Public */,
      16,    0,  173,    2, 0x06 /* Public */,
      17,    1,  174,    2, 0x06 /* Public */,
      19,    1,  177,    2, 0x06 /* Public */,
      21,    1,  180,    2, 0x06 /* Public */,
      23,    1,  183,    2, 0x06 /* Public */,
      25,    1,  186,    2, 0x06 /* Public */,
      27,    1,  189,    2, 0x06 /* Public */,
      29,    1,  192,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      31,    0,  195,    2, 0x0a /* Public */,
      32,    1,  196,    2, 0x0a /* Public */,
      33,    1,  199,    2, 0x0a /* Public */,
      35,    0,  202,    2, 0x0a /* Public */,
      36,    0,  203,    2, 0x0a /* Public */,
      37,    1,  204,    2, 0x0a /* Public */,
      39,    1,  207,    2, 0x0a /* Public */,
      41,    1,  210,    2, 0x0a /* Public */,
      43,    1,  213,    2, 0x08 /* Private */,
      45,    1,  216,    2, 0x08 /* Private */,
      46,    0,  219,    2, 0x08 /* Private */,
      47,    0,  220,    2, 0x08 /* Private */,
      48,    0,  221,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   18,
    QMetaType::Void, QMetaType::QString,   20,
    QMetaType::Void, QMetaType::Int,   22,
    QMetaType::Void, QMetaType::Int,   24,
    QMetaType::Void, 0x80000000 | 26,    4,
    QMetaType::Void, QMetaType::Int,   28,
    QMetaType::Void, QMetaType::Int,   30,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,   34,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   38,
    QMetaType::Void, QMetaType::QString,   40,
    QMetaType::Void, QMetaType::Int,   42,
    QMetaType::Void, QMetaType::Double,   44,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void PlaybackController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PlaybackController *_t = static_cast<PlaybackController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sessionPreloaded((*reinterpret_cast< PreloadedSessionData(*)>(_a[1]))); break;
        case 1: _t->sessionPreloadFailed((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->timeChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->loading((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->percentLoaded((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: _t->designRootChanged(); break;
        case 6: _t->statusChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->finishedPlayback(); break;
        case 8: _t->loadError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->taskChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 10: _t->rewarded((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->alignPlotSig((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->sessionLoaded((*reinterpret_cast< LoadedSessionData(*)>(_a[1]))); break;
        case 13: _t->eventAddedSig((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->scriptContAddedSig((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->stop(); break;
        case 16: _t->jumpToTime((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 17: _t->setRunSpeed((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 18: _t->setUserToOperator(); break;
        case 19: _t->setUserToSubject(); break;
        case 20: _t->enableLFPLoad((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 21: _t->selectFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 22: _t->selectRun((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 23: _t->newRunLength((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 24: _t->setCurrTime((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 25: _t->playbackEnded(); break;
        case 26: _t->setup(); break;
        case 27: _t->update(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< PreloadedSessionData >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< LoadedSessionData >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PlaybackController::*_t)(PreloadedSessionData );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackController::sessionPreloaded)) {
                *result = 0;
            }
        }
        {
            typedef void (PlaybackController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackController::sessionPreloadFailed)) {
                *result = 1;
            }
        }
        {
            typedef void (PlaybackController::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackController::timeChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (PlaybackController::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackController::loading)) {
                *result = 3;
            }
        }
        {
            typedef void (PlaybackController::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackController::percentLoaded)) {
                *result = 4;
            }
        }
        {
            typedef void (PlaybackController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackController::designRootChanged)) {
                *result = 5;
            }
        }
        {
            typedef void (PlaybackController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackController::statusChanged)) {
                *result = 6;
            }
        }
        {
            typedef void (PlaybackController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackController::finishedPlayback)) {
                *result = 7;
            }
        }
        {
            typedef void (PlaybackController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackController::loadError)) {
                *result = 8;
            }
        }
        {
            typedef void (PlaybackController::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackController::taskChanged)) {
                *result = 9;
            }
        }
        {
            typedef void (PlaybackController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackController::rewarded)) {
                *result = 10;
            }
        }
        {
            typedef void (PlaybackController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackController::alignPlotSig)) {
                *result = 11;
            }
        }
        {
            typedef void (PlaybackController::*_t)(LoadedSessionData );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackController::sessionLoaded)) {
                *result = 12;
            }
        }
        {
            typedef void (PlaybackController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackController::eventAddedSig)) {
                *result = 13;
            }
        }
        {
            typedef void (PlaybackController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackController::scriptContAddedSig)) {
                *result = 14;
            }
        }
    }
}

const QMetaObject PlaybackController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_PlaybackController.data,
      qt_meta_data_PlaybackController,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PlaybackController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlaybackController::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PlaybackController.stringdata0))
        return static_cast<void*>(const_cast< PlaybackController*>(this));
    return QObject::qt_metacast(_clname);
}

int PlaybackController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 28)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 28;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 28)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 28;
    }
    return _id;
}

// SIGNAL 0
void PlaybackController::sessionPreloaded(PreloadedSessionData _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PlaybackController::sessionPreloadFailed(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PlaybackController::timeChanged(double _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void PlaybackController::loading(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void PlaybackController::percentLoaded(double _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void PlaybackController::designRootChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void PlaybackController::statusChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void PlaybackController::finishedPlayback()
{
    QMetaObject::activate(this, &staticMetaObject, 7, Q_NULLPTR);
}

// SIGNAL 8
void PlaybackController::loadError(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void PlaybackController::taskChanged(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void PlaybackController::rewarded(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void PlaybackController::alignPlotSig(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void PlaybackController::sessionLoaded(LoadedSessionData _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void PlaybackController::eventAddedSig(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void PlaybackController::scriptContAddedSig(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}
QT_END_MOC_NAMESPACE
