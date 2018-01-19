/****************************************************************************
** Meta object code from reading C++ file 'OperatorInfoGraphic.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/stimuli/OperatorInfoGraphic.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OperatorInfoGraphic.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__OperatorInfoGraphic_t {
    QByteArrayData data[15];
    char stringdata0[179];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__OperatorInfoGraphic_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__OperatorInfoGraphic_t qt_meta_stringdata_Picto__OperatorInfoGraphic = {
    {
QT_MOC_LITERAL(0, 0, 26), // "Picto::OperatorInfoGraphic"
QT_MOC_LITERAL(1, 27, 7), // "setData"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 5), // "field"
QT_MOC_LITERAL(4, 42, 5), // "value"
QT_MOC_LITERAL(5, 48, 12), // "getDataAsInt"
QT_MOC_LITERAL(6, 61, 15), // "getDataAsString"
QT_MOC_LITERAL(7, 77, 15), // "getDataAsDouble"
QT_MOC_LITERAL(8, 93, 12), // "getDataAsNum"
QT_MOC_LITERAL(9, 106, 9), // "getFields"
QT_MOC_LITERAL(10, 116, 9), // "getValues"
QT_MOC_LITERAL(11, 126, 13), // "getNumEntries"
QT_MOC_LITERAL(12, 140, 11), // "updateValue"
QT_MOC_LITERAL(13, 152, 16), // "propValueChanged"
QT_MOC_LITERAL(14, 169, 9) // "Property*"

    },
    "Picto::OperatorInfoGraphic\0setData\0\0"
    "field\0value\0getDataAsInt\0getDataAsString\0"
    "getDataAsDouble\0getDataAsNum\0getFields\0"
    "getValues\0getNumEntries\0updateValue\0"
    "propValueChanged\0Property*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__OperatorInfoGraphic[] = {

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
       1,    2,   64,    2, 0x0a /* Public */,
       5,    1,   69,    2, 0x0a /* Public */,
       6,    1,   72,    2, 0x0a /* Public */,
       7,    1,   75,    2, 0x0a /* Public */,
       8,    1,   78,    2, 0x0a /* Public */,
       9,    0,   81,    2, 0x0a /* Public */,
      10,    0,   82,    2, 0x0a /* Public */,
      11,    0,   83,    2, 0x0a /* Public */,
      12,    0,   84,    2, 0x0a /* Public */,
      13,    2,   85,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,    3,    4,
    QMetaType::Int, QMetaType::QString,    3,
    QMetaType::QString, QMetaType::QString,    3,
    QMetaType::Double, QMetaType::QString,    3,
    QMetaType::Double, QMetaType::QString,    3,
    QMetaType::QVariantList,
    QMetaType::QVariantList,
    QMetaType::Int,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14, QMetaType::QVariant,    2,    2,

       0        // eod
};

void Picto::OperatorInfoGraphic::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OperatorInfoGraphic *_t = static_cast<OperatorInfoGraphic *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setData((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        case 1: { int _r = _t->getDataAsInt((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 2: { QString _r = _t->getDataAsString((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 3: { double _r = _t->getDataAsDouble((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 4: { double _r = _t->getDataAsNum((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 5: { QVariantList _r = _t->getFields();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 6: { QVariantList _r = _t->getValues();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 7: { int _r = _t->getNumEntries();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 8: _t->updateValue(); break;
        case 9: _t->propValueChanged((*reinterpret_cast< Property*(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Property* >(); break;
            }
            break;
        }
    }
}

const QMetaObject Picto::OperatorInfoGraphic::staticMetaObject = {
    { &VisualElement::staticMetaObject, qt_meta_stringdata_Picto__OperatorInfoGraphic.data,
      qt_meta_data_Picto__OperatorInfoGraphic,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::OperatorInfoGraphic::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::OperatorInfoGraphic::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__OperatorInfoGraphic.stringdata0))
        return static_cast<void*>(const_cast< OperatorInfoGraphic*>(this));
    return VisualElement::qt_metacast(_clname);
}

int Picto::OperatorInfoGraphic::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = VisualElement::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
