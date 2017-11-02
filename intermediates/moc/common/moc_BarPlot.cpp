/****************************************************************************
** Meta object code from reading C++ file 'BarPlot.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/operator/BarPlot.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BarPlot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__BarPlot_t {
    QByteArrayData data[14];
    char stringdata0[133];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__BarPlot_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__BarPlot_t qt_meta_stringdata_Picto__BarPlot = {
    {
QT_MOC_LITERAL(0, 0, 14), // "Picto::BarPlot"
QT_MOC_LITERAL(1, 15, 20), // "initializeBarPlotSig"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 15), // "BarAxisHandler*"
QT_MOC_LITERAL(4, 53, 8), // "pHandler"
QT_MOC_LITERAL(5, 62, 11), // "adjustValue"
QT_MOC_LITERAL(6, 74, 3), // "bin"
QT_MOC_LITERAL(7, 78, 5), // "value"
QT_MOC_LITERAL(8, 84, 8), // "setValue"
QT_MOC_LITERAL(9, 93, 8), // "getValue"
QT_MOC_LITERAL(10, 102, 8), // "getLabel"
QT_MOC_LITERAL(11, 111, 8), // "setLabel"
QT_MOC_LITERAL(12, 120, 4), // "name"
QT_MOC_LITERAL(13, 125, 7) // "dropBin"

    },
    "Picto::BarPlot\0initializeBarPlotSig\0"
    "\0BarAxisHandler*\0pHandler\0adjustValue\0"
    "bin\0value\0setValue\0getValue\0getLabel\0"
    "setLabel\0name\0dropBin"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__BarPlot[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    2,   52,    2, 0x0a /* Public */,
       8,    2,   57,    2, 0x0a /* Public */,
       9,    1,   62,    2, 0x0a /* Public */,
      10,    1,   65,    2, 0x0a /* Public */,
      11,    2,   68,    2, 0x0a /* Public */,
      13,    1,   73,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::Long, QMetaType::Double,    6,    7,
    QMetaType::Void, QMetaType::Long, QMetaType::Double,    6,    7,
    QMetaType::Double, QMetaType::Long,    6,
    QMetaType::QString, QMetaType::Long,    6,
    QMetaType::Void, QMetaType::Long, QMetaType::QString,    6,   12,
    QMetaType::Void, QMetaType::Long,    6,

       0        // eod
};

void Picto::BarPlot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BarPlot *_t = static_cast<BarPlot *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->initializeBarPlotSig((*reinterpret_cast< BarAxisHandler*(*)>(_a[1]))); break;
        case 1: _t->adjustValue((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 2: _t->setValue((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 3: { double _r = _t->getValue((*reinterpret_cast< long(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 4: { QString _r = _t->getLabel((*reinterpret_cast< long(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 5: _t->setLabel((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 6: _t->dropBin((*reinterpret_cast< long(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (BarPlot::*_t)(BarAxisHandler * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BarPlot::initializeBarPlotSig)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject Picto::BarPlot::staticMetaObject = {
    { &BarBase::staticMetaObject, qt_meta_stringdata_Picto__BarPlot.data,
      qt_meta_data_Picto__BarPlot,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::BarPlot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::BarPlot::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__BarPlot.stringdata0))
        return static_cast<void*>(const_cast< BarPlot*>(this));
    return BarBase::qt_metacast(_clname);
}

int Picto::BarPlot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BarBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void Picto::BarPlot::initializeBarPlotSig(BarAxisHandler * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
