/****************************************************************************
** Meta object code from reading C++ file 'SessionPlayer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/playback/SessionPlayer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SessionPlayer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__SessionPlayer_t {
    QByteArrayData data[14];
    char stringdata0[136];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__SessionPlayer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__SessionPlayer_t qt_meta_stringdata_Picto__SessionPlayer = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Picto::SessionPlayer"
QT_MOC_LITERAL(1, 21, 11), // "startingRun"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 8), // "taskName"
QT_MOC_LITERAL(4, 43, 7), // "runName"
QT_MOC_LITERAL(5, 51, 7), // "loading"
QT_MOC_LITERAL(6, 59, 12), // "startingLoad"
QT_MOC_LITERAL(7, 72, 10), // "reachedEnd"
QT_MOC_LITERAL(8, 83, 10), // "spikeEvent"
QT_MOC_LITERAL(9, 94, 4), // "time"
QT_MOC_LITERAL(10, 99, 7), // "channel"
QT_MOC_LITERAL(11, 107, 4), // "unit"
QT_MOC_LITERAL(12, 112, 14), // "QVector<float>"
QT_MOC_LITERAL(13, 127, 8) // "waveform"

    },
    "Picto::SessionPlayer\0startingRun\0\0"
    "taskName\0runName\0loading\0startingLoad\0"
    "reachedEnd\0spikeEvent\0time\0channel\0"
    "unit\0QVector<float>\0waveform"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__SessionPlayer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x06 /* Public */,
       5,    1,   39,    2, 0x06 /* Public */,
       7,    0,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    4,   43,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Double, QMetaType::Int, QMetaType::Int, 0x80000000 | 12,    9,   10,   11,   13,

       0        // eod
};

void Picto::SessionPlayer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SessionPlayer *_t = static_cast<SessionPlayer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->startingRun((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->loading((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->reachedEnd(); break;
        case 3: _t->spikeEvent((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< QVector<float>(*)>(_a[4]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 3:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<float> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SessionPlayer::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SessionPlayer::startingRun)) {
                *result = 0;
            }
        }
        {
            typedef void (SessionPlayer::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SessionPlayer::loading)) {
                *result = 1;
            }
        }
        {
            typedef void (SessionPlayer::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SessionPlayer::reachedEnd)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject Picto::SessionPlayer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Picto__SessionPlayer.data,
      qt_meta_data_Picto__SessionPlayer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::SessionPlayer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::SessionPlayer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__SessionPlayer.stringdata0))
        return static_cast<void*>(const_cast< SessionPlayer*>(this));
    return QObject::qt_metacast(_clname);
}

int Picto::SessionPlayer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Picto::SessionPlayer::startingRun(QString _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Picto::SessionPlayer::loading(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Picto::SessionPlayer::reachedEnd()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
