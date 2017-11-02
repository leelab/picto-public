/****************************************************************************
** Meta object code from reading C++ file 'SpikeState.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/playback/SpikeState.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SpikeState.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__SpikeState_t {
    QByteArrayData data[8];
    char stringdata0[72];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__SpikeState_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__SpikeState_t qt_meta_stringdata_Picto__SpikeState = {
    {
QT_MOC_LITERAL(0, 0, 17), // "Picto::SpikeState"
QT_MOC_LITERAL(1, 18, 10), // "spikeEvent"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 4), // "time"
QT_MOC_LITERAL(4, 35, 7), // "channel"
QT_MOC_LITERAL(5, 43, 4), // "unit"
QT_MOC_LITERAL(6, 48, 14), // "QVector<float>"
QT_MOC_LITERAL(7, 63, 8) // "waveform"

    },
    "Picto::SpikeState\0spikeEvent\0\0time\0"
    "channel\0unit\0QVector<float>\0waveform"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__SpikeState[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    4,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double, QMetaType::Int, QMetaType::Int, 0x80000000 | 6,    3,    4,    5,    7,

       0        // eod
};

void Picto::SpikeState::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SpikeState *_t = static_cast<SpikeState *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->spikeEvent((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< QVector<float>(*)>(_a[4]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
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
            typedef void (SpikeState::*_t)(double , int , int , QVector<float> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SpikeState::spikeEvent)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject Picto::SpikeState::staticMetaObject = {
    { &SpikeReader::staticMetaObject, qt_meta_stringdata_Picto__SpikeState.data,
      qt_meta_data_Picto__SpikeState,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::SpikeState::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::SpikeState::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__SpikeState.stringdata0))
        return static_cast<void*>(const_cast< SpikeState*>(this));
    if (!strcmp(_clname, "DataState"))
        return static_cast< DataState*>(const_cast< SpikeState*>(this));
    return SpikeReader::qt_metacast(_clname);
}

int Picto::SpikeState::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SpikeReader::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Picto::SpikeState::spikeEvent(double _t1, int _t2, int _t3, QVector<float> _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
