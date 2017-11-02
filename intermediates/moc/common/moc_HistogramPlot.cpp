/****************************************************************************
** Meta object code from reading C++ file 'HistogramPlot.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/operator/HistogramPlot.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HistogramPlot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__HistogramPlot_t {
    QByteArrayData data[8];
    char stringdata0[70];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__HistogramPlot_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__HistogramPlot_t qt_meta_stringdata_Picto__HistogramPlot = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Picto::HistogramPlot"
QT_MOC_LITERAL(1, 21, 11), // "adjustValue"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 3), // "bin"
QT_MOC_LITERAL(4, 38, 5), // "value"
QT_MOC_LITERAL(5, 44, 8), // "setValue"
QT_MOC_LITERAL(6, 53, 8), // "getValue"
QT_MOC_LITERAL(7, 62, 7) // "dropBin"

    },
    "Picto::HistogramPlot\0adjustValue\0\0bin\0"
    "value\0setValue\0getValue\0dropBin"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__HistogramPlot[] = {

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
       1,    2,   34,    2, 0x0a /* Public */,
       5,    2,   39,    2, 0x0a /* Public */,
       6,    1,   44,    2, 0x0a /* Public */,
       7,    1,   47,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Double, QMetaType::Double,    3,    4,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,    3,    4,
    QMetaType::Double, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::Double,    3,

       0        // eod
};

void Picto::HistogramPlot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HistogramPlot *_t = static_cast<HistogramPlot *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->adjustValue((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 1: _t->setValue((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 2: { double _r = _t->getValue((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 3: _t->dropBin((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject Picto::HistogramPlot::staticMetaObject = {
    { &BarBase::staticMetaObject, qt_meta_stringdata_Picto__HistogramPlot.data,
      qt_meta_data_Picto__HistogramPlot,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::HistogramPlot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::HistogramPlot::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__HistogramPlot.stringdata0))
        return static_cast<void*>(const_cast< HistogramPlot*>(this));
    return BarBase::qt_metacast(_clname);
}

int Picto::HistogramPlot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BarBase::qt_metacall(_c, _id, _a);
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
