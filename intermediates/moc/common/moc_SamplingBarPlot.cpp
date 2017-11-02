/****************************************************************************
** Meta object code from reading C++ file 'SamplingBarPlot.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/operator/SamplingBarPlot.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SamplingBarPlot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__SamplingBarPlot_t {
    QByteArrayData data[19];
    char stringdata0[252];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__SamplingBarPlot_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__SamplingBarPlot_t qt_meta_stringdata_Picto__SamplingBarPlot = {
    {
QT_MOC_LITERAL(0, 0, 22), // "Picto::SamplingBarPlot"
QT_MOC_LITERAL(1, 23, 28), // "initializeSamplingBarPlotSig"
QT_MOC_LITERAL(2, 52, 0), // ""
QT_MOC_LITERAL(3, 53, 15), // "BarAxisHandler*"
QT_MOC_LITERAL(4, 69, 8), // "pHandler"
QT_MOC_LITERAL(5, 78, 11), // "submitValue"
QT_MOC_LITERAL(6, 90, 3), // "bin"
QT_MOC_LITERAL(7, 94, 5), // "value"
QT_MOC_LITERAL(8, 100, 8), // "getValue"
QT_MOC_LITERAL(9, 109, 18), // "setCumulativeValue"
QT_MOC_LITERAL(10, 128, 18), // "getCumulativeValue"
QT_MOC_LITERAL(11, 147, 10), // "setSamples"
QT_MOC_LITERAL(12, 158, 10), // "getSamples"
QT_MOC_LITERAL(13, 169, 25), // "setCumulativeValueSquared"
QT_MOC_LITERAL(14, 195, 25), // "getCumulativeValueSquared"
QT_MOC_LITERAL(15, 221, 8), // "getLabel"
QT_MOC_LITERAL(16, 230, 8), // "setLabel"
QT_MOC_LITERAL(17, 239, 4), // "name"
QT_MOC_LITERAL(18, 244, 7) // "dropBin"

    },
    "Picto::SamplingBarPlot\0"
    "initializeSamplingBarPlotSig\0\0"
    "BarAxisHandler*\0pHandler\0submitValue\0"
    "bin\0value\0getValue\0setCumulativeValue\0"
    "getCumulativeValue\0setSamples\0getSamples\0"
    "setCumulativeValueSquared\0"
    "getCumulativeValueSquared\0getLabel\0"
    "setLabel\0name\0dropBin"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__SamplingBarPlot[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    2,   77,    2, 0x0a /* Public */,
       8,    1,   82,    2, 0x0a /* Public */,
       9,    2,   85,    2, 0x0a /* Public */,
      10,    1,   90,    2, 0x0a /* Public */,
      11,    2,   93,    2, 0x0a /* Public */,
      12,    1,   98,    2, 0x0a /* Public */,
      13,    2,  101,    2, 0x0a /* Public */,
      14,    1,  106,    2, 0x0a /* Public */,
      15,    1,  109,    2, 0x0a /* Public */,
      16,    2,  112,    2, 0x0a /* Public */,
      18,    1,  117,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::Long, QMetaType::Double,    6,    7,
    QMetaType::Double, QMetaType::Long,    6,
    QMetaType::Void, QMetaType::Long, QMetaType::Double,    6,    7,
    QMetaType::Double, QMetaType::Long,    6,
    QMetaType::Void, QMetaType::Long, QMetaType::Long,    6,    7,
    QMetaType::Long, QMetaType::Long,    6,
    QMetaType::Void, QMetaType::Long, QMetaType::Double,    6,    7,
    QMetaType::Double, QMetaType::Long,    6,
    QMetaType::QString, QMetaType::Long,    6,
    QMetaType::Void, QMetaType::Long, QMetaType::QString,    6,   17,
    QMetaType::Void, QMetaType::Long,    6,

       0        // eod
};

void Picto::SamplingBarPlot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SamplingBarPlot *_t = static_cast<SamplingBarPlot *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->initializeSamplingBarPlotSig((*reinterpret_cast< BarAxisHandler*(*)>(_a[1]))); break;
        case 1: _t->submitValue((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 2: { double _r = _t->getValue((*reinterpret_cast< long(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 3: _t->setCumulativeValue((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 4: { double _r = _t->getCumulativeValue((*reinterpret_cast< long(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 5: _t->setSamples((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< long(*)>(_a[2]))); break;
        case 6: { long _r = _t->getSamples((*reinterpret_cast< long(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< long*>(_a[0]) = _r; }  break;
        case 7: _t->setCumulativeValueSquared((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 8: { double _r = _t->getCumulativeValueSquared((*reinterpret_cast< long(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 9: { QString _r = _t->getLabel((*reinterpret_cast< long(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 10: _t->setLabel((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 11: _t->dropBin((*reinterpret_cast< long(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SamplingBarPlot::*_t)(BarAxisHandler * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SamplingBarPlot::initializeSamplingBarPlotSig)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject Picto::SamplingBarPlot::staticMetaObject = {
    { &SamplingBarBase::staticMetaObject, qt_meta_stringdata_Picto__SamplingBarPlot.data,
      qt_meta_data_Picto__SamplingBarPlot,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::SamplingBarPlot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::SamplingBarPlot::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__SamplingBarPlot.stringdata0))
        return static_cast<void*>(const_cast< SamplingBarPlot*>(this));
    return SamplingBarBase::qt_metacast(_clname);
}

int Picto::SamplingBarPlot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SamplingBarBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void Picto::SamplingBarPlot::initializeSamplingBarPlotSig(BarAxisHandler * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
