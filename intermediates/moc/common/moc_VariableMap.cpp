/****************************************************************************
** Meta object code from reading C++ file 'VariableMap.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/variable/VariableMap.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VariableMap.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__VariableMap_t {
    QByteArrayData data[17];
    char stringdata0[180];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__VariableMap_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__VariableMap_t qt_meta_stringdata_Picto__VariableMap = {
    {
QT_MOC_LITERAL(0, 0, 18), // "Picto::VariableMap"
QT_MOC_LITERAL(1, 19, 6), // "length"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 8), // "setValue"
QT_MOC_LITERAL(4, 36, 3), // "key"
QT_MOC_LITERAL(5, 40, 5), // "value"
QT_MOC_LITERAL(6, 46, 14), // "fromAssocArray"
QT_MOC_LITERAL(7, 61, 10), // "assocArray"
QT_MOC_LITERAL(8, 72, 13), // "getValueAsNum"
QT_MOC_LITERAL(9, 86, 16), // "getValueAsString"
QT_MOC_LITERAL(10, 103, 7), // "getKeys"
QT_MOC_LITERAL(11, 111, 11), // "takeAtAsNum"
QT_MOC_LITERAL(12, 123, 14), // "takeAtAsString"
QT_MOC_LITERAL(13, 138, 8), // "removeAt"
QT_MOC_LITERAL(14, 147, 5), // "clear"
QT_MOC_LITERAL(15, 153, 16), // "propValueChanged"
QT_MOC_LITERAL(16, 170, 9) // "Property*"

    },
    "Picto::VariableMap\0length\0\0setValue\0"
    "key\0value\0fromAssocArray\0assocArray\0"
    "getValueAsNum\0getValueAsString\0getKeys\0"
    "takeAtAsNum\0takeAtAsString\0removeAt\0"
    "clear\0propValueChanged\0Property*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__VariableMap[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x0a /* Public */,
       3,    2,   70,    2, 0x0a /* Public */,
       6,    1,   75,    2, 0x0a /* Public */,
       8,    1,   78,    2, 0x0a /* Public */,
       9,    1,   81,    2, 0x0a /* Public */,
      10,    0,   84,    2, 0x0a /* Public */,
      11,    1,   85,    2, 0x0a /* Public */,
      12,    1,   88,    2, 0x0a /* Public */,
      13,    1,   91,    2, 0x0a /* Public */,
      14,    0,   94,    2, 0x0a /* Public */,
      15,    2,   95,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Int,
    QMetaType::Void, QMetaType::QVariant, QMetaType::QVariant,    4,    5,
    QMetaType::Void, QMetaType::QVariantMap,    7,
    QMetaType::Double, QMetaType::QVariant,    4,
    QMetaType::QString, QMetaType::QVariant,    4,
    QMetaType::QVariantList,
    QMetaType::Double, QMetaType::QVariant,    4,
    QMetaType::QString, QMetaType::QVariant,    4,
    QMetaType::Void, QMetaType::QVariant,    4,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16, QMetaType::QVariant,    2,    2,

       0        // eod
};

void Picto::VariableMap::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        VariableMap *_t = static_cast<VariableMap *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { int _r = _t->length();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 1: _t->setValue((*reinterpret_cast< QVariant(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 2: _t->fromAssocArray((*reinterpret_cast< QVariantMap(*)>(_a[1]))); break;
        case 3: { double _r = _t->getValueAsNum((*reinterpret_cast< QVariant(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 4: { QString _r = _t->getValueAsString((*reinterpret_cast< QVariant(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 5: { QVariantList _r = _t->getKeys();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 6: { double _r = _t->takeAtAsNum((*reinterpret_cast< QVariant(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 7: { QString _r = _t->takeAtAsString((*reinterpret_cast< QVariant(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 8: _t->removeAt((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 9: _t->clear(); break;
        case 10: _t->propValueChanged((*reinterpret_cast< Property*(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Property* >(); break;
            }
            break;
        }
    }
}

const QMetaObject Picto::VariableMap::staticMetaObject = {
    { &Variable::staticMetaObject, qt_meta_stringdata_Picto__VariableMap.data,
      qt_meta_data_Picto__VariableMap,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::VariableMap::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::VariableMap::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__VariableMap.stringdata0))
        return static_cast<void*>(const_cast< VariableMap*>(this));
    return Variable::qt_metacast(_clname);
}

int Picto::VariableMap::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Variable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
