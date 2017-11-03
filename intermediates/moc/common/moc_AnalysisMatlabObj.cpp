/****************************************************************************
** Meta object code from reading C++ file 'AnalysisMatlabObj.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/parameter/AnalysisMatlabObj.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AnalysisMatlabObj.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__AnalysisMatlabObj_t {
    QByteArrayData data[12];
    char stringdata0[79];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__AnalysisMatlabObj_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__AnalysisMatlabObj_t qt_meta_stringdata_Picto__AnalysisMatlabObj = {
    {
QT_MOC_LITERAL(0, 0, 24), // "Picto::AnalysisMatlabObj"
QT_MOC_LITERAL(1, 25, 7), // "binocdf"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 3), // "in1"
QT_MOC_LITERAL(4, 38, 3), // "in2"
QT_MOC_LITERAL(5, 42, 3), // "in3"
QT_MOC_LITERAL(6, 46, 7), // "binopdf"
QT_MOC_LITERAL(7, 54, 4), // "rand"
QT_MOC_LITERAL(8, 59, 2), // "in"
QT_MOC_LITERAL(9, 62, 7), // "normrnd"
QT_MOC_LITERAL(10, 70, 2), // "mu"
QT_MOC_LITERAL(11, 73, 5) // "sigma"

    },
    "Picto::AnalysisMatlabObj\0binocdf\0\0in1\0"
    "in2\0in3\0binopdf\0rand\0in\0normrnd\0mu\0"
    "sigma"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__AnalysisMatlabObj[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    3,   34,    2, 0x0a /* Public */,
       6,    3,   41,    2, 0x0a /* Public */,
       7,    1,   48,    2, 0x0a /* Public */,
       9,    2,   51,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::QVariantList, QMetaType::QVariantList, QMetaType::QVariantList, QMetaType::QVariantList,    3,    4,    5,
    QMetaType::QVariantList, QMetaType::QVariantList, QMetaType::QVariantList, QMetaType::QVariantList,    3,    4,    5,
    QMetaType::QVariantList, QMetaType::Double,    8,
    QMetaType::QVariantList, QMetaType::QVariantList, QMetaType::QVariantList,   10,   11,

       0        // eod
};

void Picto::AnalysisMatlabObj::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AnalysisMatlabObj *_t = static_cast<AnalysisMatlabObj *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { QVariantList _r = _t->binocdf((*reinterpret_cast< QVariantList(*)>(_a[1])),(*reinterpret_cast< QVariantList(*)>(_a[2])),(*reinterpret_cast< QVariantList(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 1: { QVariantList _r = _t->binopdf((*reinterpret_cast< QVariantList(*)>(_a[1])),(*reinterpret_cast< QVariantList(*)>(_a[2])),(*reinterpret_cast< QVariantList(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 2: { QVariantList _r = _t->rand((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 3: { QVariantList _r = _t->normrnd((*reinterpret_cast< QVariantList(*)>(_a[1])),(*reinterpret_cast< QVariantList(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObject Picto::AnalysisMatlabObj::staticMetaObject = {
    { &AnalysisVariable::staticMetaObject, qt_meta_stringdata_Picto__AnalysisMatlabObj.data,
      qt_meta_data_Picto__AnalysisMatlabObj,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::AnalysisMatlabObj::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::AnalysisMatlabObj::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__AnalysisMatlabObj.stringdata0))
        return static_cast<void*>(const_cast< AnalysisMatlabObj*>(this));
    return AnalysisVariable::qt_metacast(_clname);
}

int Picto::AnalysisMatlabObj::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AnalysisVariable::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
