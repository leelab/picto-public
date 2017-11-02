/****************************************************************************
** Meta object code from reading C++ file 'AnalysisVariableMap.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/parameter/AnalysisVariableMap.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AnalysisVariableMap.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__AnalysisVariableMap_t {
    QByteArrayData data[15];
    char stringdata0[161];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__AnalysisVariableMap_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__AnalysisVariableMap_t qt_meta_stringdata_Picto__AnalysisVariableMap = {
    {
QT_MOC_LITERAL(0, 0, 26), // "Picto::AnalysisVariableMap"
QT_MOC_LITERAL(1, 27, 6), // "length"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 8), // "setValue"
QT_MOC_LITERAL(4, 44, 3), // "key"
QT_MOC_LITERAL(5, 48, 5), // "value"
QT_MOC_LITERAL(6, 54, 14), // "fromAssocArray"
QT_MOC_LITERAL(7, 69, 10), // "assocArray"
QT_MOC_LITERAL(8, 80, 13), // "getValueAsNum"
QT_MOC_LITERAL(9, 94, 16), // "getValueAsString"
QT_MOC_LITERAL(10, 111, 7), // "getKeys"
QT_MOC_LITERAL(11, 119, 11), // "takeAtAsNum"
QT_MOC_LITERAL(12, 131, 14), // "takeAtAsString"
QT_MOC_LITERAL(13, 146, 8), // "removeAt"
QT_MOC_LITERAL(14, 155, 5) // "clear"

    },
    "Picto::AnalysisVariableMap\0length\0\0"
    "setValue\0key\0value\0fromAssocArray\0"
    "assocArray\0getValueAsNum\0getValueAsString\0"
    "getKeys\0takeAtAsNum\0takeAtAsString\0"
    "removeAt\0clear"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__AnalysisVariableMap[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x0a /* Public */,
       3,    2,   65,    2, 0x0a /* Public */,
       6,    1,   70,    2, 0x0a /* Public */,
       8,    1,   73,    2, 0x0a /* Public */,
       9,    1,   76,    2, 0x0a /* Public */,
      10,    0,   79,    2, 0x0a /* Public */,
      11,    1,   80,    2, 0x0a /* Public */,
      12,    1,   83,    2, 0x0a /* Public */,
      13,    1,   86,    2, 0x0a /* Public */,
      14,    0,   89,    2, 0x0a /* Public */,

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

       0        // eod
};

void Picto::AnalysisVariableMap::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AnalysisVariableMap *_t = static_cast<AnalysisVariableMap *>(_o);
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
        default: ;
        }
    }
}

const QMetaObject Picto::AnalysisVariableMap::staticMetaObject = {
    { &AnalysisVariable::staticMetaObject, qt_meta_stringdata_Picto__AnalysisVariableMap.data,
      qt_meta_data_Picto__AnalysisVariableMap,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::AnalysisVariableMap::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::AnalysisVariableMap::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__AnalysisVariableMap.stringdata0))
        return static_cast<void*>(const_cast< AnalysisVariableMap*>(this));
    return AnalysisVariable::qt_metacast(_clname);
}

int Picto::AnalysisVariableMap::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AnalysisVariable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
