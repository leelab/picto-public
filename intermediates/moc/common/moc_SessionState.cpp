/****************************************************************************
** Meta object code from reading C++ file 'SessionState.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/playback/SessionState.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SessionState.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__SessionState_t {
    QByteArrayData data[25];
    char stringdata0[283];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__SessionState_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__SessionState_t qt_meta_stringdata_Picto__SessionState = {
    {
QT_MOC_LITERAL(0, 0, 19), // "Picto::SessionState"
QT_MOC_LITERAL(1, 20, 15), // "propertyChanged"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 6), // "propId"
QT_MOC_LITERAL(4, 44, 5), // "value"
QT_MOC_LITERAL(5, 50, 24), // "propertyInitValueChanged"
QT_MOC_LITERAL(6, 75, 19), // "transitionActivated"
QT_MOC_LITERAL(7, 95, 7), // "transId"
QT_MOC_LITERAL(8, 103, 14), // "framePresented"
QT_MOC_LITERAL(9, 118, 4), // "time"
QT_MOC_LITERAL(10, 123, 14), // "rewardSupplied"
QT_MOC_LITERAL(11, 138, 8), // "duration"
QT_MOC_LITERAL(12, 147, 7), // "channel"
QT_MOC_LITERAL(13, 155, 13), // "signalChanged"
QT_MOC_LITERAL(14, 169, 4), // "name"
QT_MOC_LITERAL(15, 174, 12), // "subChanNames"
QT_MOC_LITERAL(16, 187, 14), // "QVector<float>"
QT_MOC_LITERAL(17, 202, 4), // "vals"
QT_MOC_LITERAL(18, 207, 10), // "lfpChanged"
QT_MOC_LITERAL(19, 218, 10), // "spikeEvent"
QT_MOC_LITERAL(20, 229, 4), // "unit"
QT_MOC_LITERAL(21, 234, 8), // "waveform"
QT_MOC_LITERAL(22, 243, 13), // "percentLoaded"
QT_MOC_LITERAL(23, 257, 7), // "percent"
QT_MOC_LITERAL(24, 265, 17) // "lfpLoadingUpdated"

    },
    "Picto::SessionState\0propertyChanged\0"
    "\0propId\0value\0propertyInitValueChanged\0"
    "transitionActivated\0transId\0framePresented\0"
    "time\0rewardSupplied\0duration\0channel\0"
    "signalChanged\0name\0subChanNames\0"
    "QVector<float>\0vals\0lfpChanged\0"
    "spikeEvent\0unit\0waveform\0percentLoaded\0"
    "percent\0lfpLoadingUpdated"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__SessionState[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   64,    2, 0x06 /* Public */,
       5,    2,   69,    2, 0x06 /* Public */,
       6,    1,   74,    2, 0x06 /* Public */,
       8,    1,   77,    2, 0x06 /* Public */,
      10,    3,   80,    2, 0x06 /* Public */,
      13,    3,   87,    2, 0x06 /* Public */,
      18,    2,   94,    2, 0x06 /* Public */,
      19,    4,   99,    2, 0x06 /* Public */,
      22,    1,  108,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      24,    1,  111,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Double,    9,
    QMetaType::Void, QMetaType::Double, QMetaType::Int, QMetaType::Int,    9,   11,   12,
    QMetaType::Void, QMetaType::QString, QMetaType::QStringList, 0x80000000 | 16,   14,   15,   17,
    QMetaType::Void, QMetaType::Int, QMetaType::Double,   12,    4,
    QMetaType::Void, QMetaType::Double, QMetaType::Int, QMetaType::Int, 0x80000000 | 16,    9,   12,   20,   21,
    QMetaType::Void, QMetaType::Double,   23,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,   23,

       0        // eod
};

void Picto::SessionState::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SessionState *_t = static_cast<SessionState *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->propertyChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->propertyInitValueChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 2: _t->transitionActivated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->framePresented((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->rewardSupplied((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 5: _t->signalChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QStringList(*)>(_a[2])),(*reinterpret_cast< QVector<float>(*)>(_a[3]))); break;
        case 6: _t->lfpChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 7: _t->spikeEvent((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< QVector<float>(*)>(_a[4]))); break;
        case 8: _t->percentLoaded((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->lfpLoadingUpdated((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<float> >(); break;
            }
            break;
        case 7:
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
            typedef void (SessionState::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SessionState::propertyChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (SessionState::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SessionState::propertyInitValueChanged)) {
                *result = 1;
            }
        }
        {
            typedef void (SessionState::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SessionState::transitionActivated)) {
                *result = 2;
            }
        }
        {
            typedef void (SessionState::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SessionState::framePresented)) {
                *result = 3;
            }
        }
        {
            typedef void (SessionState::*_t)(double , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SessionState::rewardSupplied)) {
                *result = 4;
            }
        }
        {
            typedef void (SessionState::*_t)(QString , QStringList , QVector<float> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SessionState::signalChanged)) {
                *result = 5;
            }
        }
        {
            typedef void (SessionState::*_t)(int , double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SessionState::lfpChanged)) {
                *result = 6;
            }
        }
        {
            typedef void (SessionState::*_t)(double , int , int , QVector<float> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SessionState::spikeEvent)) {
                *result = 7;
            }
        }
        {
            typedef void (SessionState::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SessionState::percentLoaded)) {
                *result = 8;
            }
        }
    }
}

const QMetaObject Picto::SessionState::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Picto__SessionState.data,
      qt_meta_data_Picto__SessionState,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::SessionState::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::SessionState::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__SessionState.stringdata0))
        return static_cast<void*>(const_cast< SessionState*>(this));
    return QObject::qt_metacast(_clname);
}

int Picto::SessionState::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void Picto::SessionState::propertyChanged(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Picto::SessionState::propertyInitValueChanged(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Picto::SessionState::transitionActivated(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Picto::SessionState::framePresented(double _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Picto::SessionState::rewardSupplied(double _t1, int _t2, int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Picto::SessionState::signalChanged(QString _t1, QStringList _t2, QVector<float> _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Picto::SessionState::lfpChanged(int _t1, double _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Picto::SessionState::spikeEvent(double _t1, int _t2, int _t3, QVector<float> _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Picto::SessionState::percentLoaded(double _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
QT_END_MOC_NAMESPACE
