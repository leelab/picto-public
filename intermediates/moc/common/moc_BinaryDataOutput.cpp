/****************************************************************************
** Meta object code from reading C++ file 'BinaryDataOutput.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/stimuli/BinaryDataOutput.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BinaryDataOutput.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__BinaryDataOutput_t {
    QByteArrayData data[2];
    char stringdata0[30];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__BinaryDataOutput_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__BinaryDataOutput_t qt_meta_stringdata_Picto__BinaryDataOutput = {
    {
QT_MOC_LITERAL(0, 0, 23), // "Picto::BinaryDataOutput"
QT_MOC_LITERAL(1, 24, 5) // "value"

    },
    "Picto::BinaryDataOutput\0value"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__BinaryDataOutput[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       1,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::Int, 0x00095003,

       0        // eod
};

void Picto::BinaryDataOutput::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        BinaryDataOutput *_t = static_cast<BinaryDataOutput *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getBinValue(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        BinaryDataOutput *_t = static_cast<BinaryDataOutput *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setBinValue(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Picto::BinaryDataOutput::staticMetaObject = {
    { &OutputSignal::staticMetaObject, qt_meta_stringdata_Picto__BinaryDataOutput.data,
      qt_meta_data_Picto__BinaryDataOutput,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::BinaryDataOutput::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::BinaryDataOutput::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__BinaryDataOutput.stringdata0))
        return static_cast<void*>(const_cast< BinaryDataOutput*>(this));
    return OutputSignal::qt_metacast(_clname);
}

int Picto::BinaryDataOutput::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = OutputSignal::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
   if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
