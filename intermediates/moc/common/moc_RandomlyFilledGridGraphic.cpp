/****************************************************************************
** Meta object code from reading C++ file 'RandomlyFilledGridGraphic.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/stimuli/RandomlyFilledGridGraphic.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RandomlyFilledGridGraphic.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__RandomlyFilledGridGraphic_t {
    QByteArrayData data[6];
    char stringdata0[92];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__RandomlyFilledGridGraphic_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__RandomlyFilledGridGraphic_t qt_meta_stringdata_Picto__RandomlyFilledGridGraphic = {
    {
QT_MOC_LITERAL(0, 0, 32), // "Picto::RandomlyFilledGridGraphic"
QT_MOC_LITERAL(1, 33, 24), // "slotPropertyValueChanged"
QT_MOC_LITERAL(2, 58, 0), // ""
QT_MOC_LITERAL(3, 59, 12), // "propertyName"
QT_MOC_LITERAL(4, 72, 5), // "index"
QT_MOC_LITERAL(5, 78, 13) // "propertyValue"

    },
    "Picto::RandomlyFilledGridGraphic\0"
    "slotPropertyValueChanged\0\0propertyName\0"
    "index\0propertyValue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__RandomlyFilledGridGraphic[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    3,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QVariant,    3,    4,    5,

       0        // eod
};

void Picto::RandomlyFilledGridGraphic::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RandomlyFilledGridGraphic *_t = static_cast<RandomlyFilledGridGraphic *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slotPropertyValueChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QVariant(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject Picto::RandomlyFilledGridGraphic::staticMetaObject = {
    { &VisualElement::staticMetaObject, qt_meta_stringdata_Picto__RandomlyFilledGridGraphic.data,
      qt_meta_data_Picto__RandomlyFilledGridGraphic,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::RandomlyFilledGridGraphic::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::RandomlyFilledGridGraphic::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__RandomlyFilledGridGraphic.stringdata0))
        return static_cast<void*>(const_cast< RandomlyFilledGridGraphic*>(this));
    return VisualElement::qt_metacast(_clname);
}

int Picto::RandomlyFilledGridGraphic::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = VisualElement::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
