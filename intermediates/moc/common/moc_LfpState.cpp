/****************************************************************************
** Meta object code from reading C++ file 'LfpState.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/playback/LfpState.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LfpState.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__LfpState_t {
    QByteArrayData data[7];
    char stringdata0[66];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__LfpState_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__LfpState_t qt_meta_stringdata_Picto__LfpState = {
    {
QT_MOC_LITERAL(0, 0, 15), // "Picto::LfpState"
QT_MOC_LITERAL(1, 16, 10), // "lfpChanged"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 7), // "channel"
QT_MOC_LITERAL(4, 36, 5), // "value"
QT_MOC_LITERAL(5, 42, 15), // "lfpLoadProgress"
QT_MOC_LITERAL(6, 58, 7) // "percent"

    },
    "Picto::LfpState\0lfpChanged\0\0channel\0"
    "value\0lfpLoadProgress\0percent"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__LfpState[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x06 /* Public */,
       5,    1,   29,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Double,    3,    4,
    QMetaType::Void, QMetaType::Int,    6,

       0        // eod
};

void Picto::LfpState::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LfpState *_t = static_cast<LfpState *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->lfpChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 1: _t->lfpLoadProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (LfpState::*_t)(int , double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LfpState::lfpChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (LfpState::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LfpState::lfpLoadProgress)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject Picto::LfpState::staticMetaObject = {
    { &LfpReader::staticMetaObject, qt_meta_stringdata_Picto__LfpState.data,
      qt_meta_data_Picto__LfpState,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::LfpState::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::LfpState::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__LfpState.stringdata0))
        return static_cast<void*>(const_cast< LfpState*>(this));
    if (!strcmp(_clname, "DataState"))
        return static_cast< DataState*>(const_cast< LfpState*>(this));
    return LfpReader::qt_metacast(_clname);
}

int Picto::LfpState::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = LfpReader::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Picto::LfpState::lfpChanged(int _t1, double _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Picto::LfpState::lfpLoadProgress(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
