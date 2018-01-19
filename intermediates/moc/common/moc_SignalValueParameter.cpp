/****************************************************************************
** Meta object code from reading C++ file 'SignalValueParameter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/parameter/SignalValueParameter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SignalValueParameter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__SignalValueParameter_t {
    QByteArrayData data[4];
    char stringdata0[49];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__SignalValueParameter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__SignalValueParameter_t qt_meta_stringdata_Picto__SignalValueParameter = {
    {
QT_MOC_LITERAL(0, 0, 27), // "Picto::SignalValueParameter"
QT_MOC_LITERAL(1, 28, 8), // "getValue"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 10) // "subChannel"

    },
    "Picto::SignalValueParameter\0getValue\0"
    "\0subChannel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__SignalValueParameter[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x0a /* Public */,
       1,    0,   27,    2, 0x2a /* Public | MethodCloned */,

 // slots: parameters
    QMetaType::Double, QMetaType::QString,    3,
    QMetaType::Double,

       0        // eod
};

void Picto::SignalValueParameter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SignalValueParameter *_t = static_cast<SignalValueParameter *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { double _r = _t->getValue((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 1: { double _r = _t->getValue();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObject Picto::SignalValueParameter::staticMetaObject = {
    { &Parameter::staticMetaObject, qt_meta_stringdata_Picto__SignalValueParameter.data,
      qt_meta_data_Picto__SignalValueParameter,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::SignalValueParameter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::SignalValueParameter::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__SignalValueParameter.stringdata0))
        return static_cast<void*>(const_cast< SignalValueParameter*>(this));
    return Parameter::qt_metacast(_clname);
}

int Picto::SignalValueParameter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Parameter::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
