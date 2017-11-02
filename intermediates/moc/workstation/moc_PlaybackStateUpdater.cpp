/****************************************************************************
** Meta object code from reading C++ file 'PlaybackStateUpdater.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/workstation/ReplayViewer/PlaybackStateUpdater.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlaybackStateUpdater.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__PlaybackStateUpdater_t {
    QByteArrayData data[10];
    char stringdata0[111];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__PlaybackStateUpdater_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__PlaybackStateUpdater_t qt_meta_stringdata_Picto__PlaybackStateUpdater = {
    {
QT_MOC_LITERAL(0, 0, 27), // "Picto::PlaybackStateUpdater"
QT_MOC_LITERAL(1, 28, 7), // "loading"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 9), // "isLoading"
QT_MOC_LITERAL(4, 47, 13), // "percentLoaded"
QT_MOC_LITERAL(5, 61, 7), // "percent"
QT_MOC_LITERAL(6, 69, 6), // "newRun"
QT_MOC_LITERAL(7, 76, 6), // "length"
QT_MOC_LITERAL(8, 83, 16), // "finishedPlayback"
QT_MOC_LITERAL(9, 100, 10) // "reachedEnd"

    },
    "Picto::PlaybackStateUpdater\0loading\0"
    "\0isLoading\0percentLoaded\0percent\0"
    "newRun\0length\0finishedPlayback\0"
    "reachedEnd"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__PlaybackStateUpdater[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    1,   42,    2, 0x06 /* Public */,
       6,    1,   45,    2, 0x06 /* Public */,
       8,    0,   48,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   49,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    7,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Picto::PlaybackStateUpdater::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PlaybackStateUpdater *_t = static_cast<PlaybackStateUpdater *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->loading((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->percentLoaded((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->newRun((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->finishedPlayback(); break;
        case 4: _t->reachedEnd(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PlaybackStateUpdater::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackStateUpdater::loading)) {
                *result = 0;
            }
        }
        {
            typedef void (PlaybackStateUpdater::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackStateUpdater::percentLoaded)) {
                *result = 1;
            }
        }
        {
            typedef void (PlaybackStateUpdater::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackStateUpdater::newRun)) {
                *result = 2;
            }
        }
        {
            typedef void (PlaybackStateUpdater::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PlaybackStateUpdater::finishedPlayback)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject Picto::PlaybackStateUpdater::staticMetaObject = {
    { &StateUpdater::staticMetaObject, qt_meta_stringdata_Picto__PlaybackStateUpdater.data,
      qt_meta_data_Picto__PlaybackStateUpdater,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::PlaybackStateUpdater::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::PlaybackStateUpdater::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__PlaybackStateUpdater.stringdata0))
        return static_cast<void*>(const_cast< PlaybackStateUpdater*>(this));
    return StateUpdater::qt_metacast(_clname);
}

int Picto::PlaybackStateUpdater::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = StateUpdater::qt_metacall(_c, _id, _a);
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
void Picto::PlaybackStateUpdater::loading(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Picto::PlaybackStateUpdater::percentLoaded(double _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Picto::PlaybackStateUpdater::newRun(double _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Picto::PlaybackStateUpdater::finishedPlayback()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
