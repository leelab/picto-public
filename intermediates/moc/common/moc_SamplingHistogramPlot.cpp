/****************************************************************************
** Meta object code from reading C++ file 'SamplingHistogramPlot.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/operator/SamplingHistogramPlot.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SamplingHistogramPlot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__SamplingHistogramPlot_t {
    QByteArrayData data[13];
    char stringdata0[181];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__SamplingHistogramPlot_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__SamplingHistogramPlot_t qt_meta_stringdata_Picto__SamplingHistogramPlot = {
    {
QT_MOC_LITERAL(0, 0, 28), // "Picto::SamplingHistogramPlot"
QT_MOC_LITERAL(1, 29, 11), // "submitValue"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 3), // "bin"
QT_MOC_LITERAL(4, 46, 5), // "value"
QT_MOC_LITERAL(5, 52, 8), // "getValue"
QT_MOC_LITERAL(6, 61, 18), // "setCumulativeValue"
QT_MOC_LITERAL(7, 80, 18), // "getCumulativeValue"
QT_MOC_LITERAL(8, 99, 10), // "setSamples"
QT_MOC_LITERAL(9, 110, 10), // "getSamples"
QT_MOC_LITERAL(10, 121, 25), // "setCumulativeValueSquared"
QT_MOC_LITERAL(11, 147, 25), // "getCumulativeValueSquared"
QT_MOC_LITERAL(12, 173, 7) // "dropBin"

    },
    "Picto::SamplingHistogramPlot\0submitValue\0"
    "\0bin\0value\0getValue\0setCumulativeValue\0"
    "getCumulativeValue\0setSamples\0getSamples\0"
    "setCumulativeValueSquared\0"
    "getCumulativeValueSquared\0dropBin"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__SamplingHistogramPlot[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   59,    2, 0x0a /* Public */,
       5,    1,   64,    2, 0x0a /* Public */,
       6,    2,   67,    2, 0x0a /* Public */,
       7,    1,   72,    2, 0x0a /* Public */,
       8,    2,   75,    2, 0x0a /* Public */,
       9,    1,   80,    2, 0x0a /* Public */,
      10,    2,   83,    2, 0x0a /* Public */,
      11,    1,   88,    2, 0x0a /* Public */,
      12,    1,   91,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Double, QMetaType::Double,    3,    4,
    QMetaType::Double, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,    3,    4,
    QMetaType::Double, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::Double, QMetaType::Long,    3,    4,
    QMetaType::Long, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,    3,    4,
    QMetaType::Double, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::Double,    3,

       0        // eod
};

void Picto::SamplingHistogramPlot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SamplingHistogramPlot *_t = static_cast<SamplingHistogramPlot *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->submitValue((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 1: { double _r = _t->getValue((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 2: _t->setCumulativeValue((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 3: { double _r = _t->getCumulativeValue((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 4: _t->setSamples((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< long(*)>(_a[2]))); break;
        case 5: { long _r = _t->getSamples((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< long*>(_a[0]) = _r; }  break;
        case 6: _t->setCumulativeValueSquared((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 7: { double _r = _t->getCumulativeValueSquared((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 8: _t->dropBin((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject Picto::SamplingHistogramPlot::staticMetaObject = {
    { &SamplingBarBase::staticMetaObject, qt_meta_stringdata_Picto__SamplingHistogramPlot.data,
      qt_meta_data_Picto__SamplingHistogramPlot,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::SamplingHistogramPlot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::SamplingHistogramPlot::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__SamplingHistogramPlot.stringdata0))
        return static_cast<void*>(const_cast< SamplingHistogramPlot*>(this));
    return SamplingBarBase::qt_metacast(_clname);
}

int Picto::SamplingHistogramPlot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SamplingBarBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
