/****************************************************************************
** Meta object code from reading C++ file 'BarPlotPlotHandler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/operator/BarPlotPlotHandler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BarPlotPlotHandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__BarPlotPlotHandler_t {
    QByteArrayData data[8];
    char stringdata0[108];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__BarPlotPlotHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__BarPlotPlotHandler_t qt_meta_stringdata_Picto__BarPlotPlotHandler = {
    {
QT_MOC_LITERAL(0, 0, 25), // "Picto::BarPlotPlotHandler"
QT_MOC_LITERAL(1, 26, 17), // "initializeBarPlot"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 15), // "BarAxisHandler*"
QT_MOC_LITERAL(4, 61, 8), // "pHandler"
QT_MOC_LITERAL(5, 70, 13), // "handleXLabels"
QT_MOC_LITERAL(6, 84, 11), // "lLowerBound"
QT_MOC_LITERAL(7, 96, 11) // "lUpperBound"

    },
    "Picto::BarPlotPlotHandler\0initializeBarPlot\0"
    "\0BarAxisHandler*\0pHandler\0handleXLabels\0"
    "lLowerBound\0lUpperBound"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__BarPlotPlotHandler[] = {

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
       5,    2,   27,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Long, QMetaType::Long,    6,    7,

       0        // eod
};

void Picto::BarPlotPlotHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BarPlotPlotHandler *_t = static_cast<BarPlotPlotHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->initializeBarPlot((*reinterpret_cast< BarAxisHandler*(*)>(_a[1]))); break;
        case 1: _t->handleXLabels((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< long(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject Picto::BarPlotPlotHandler::staticMetaObject = {
    { &BarBasePlotHandler::staticMetaObject, qt_meta_stringdata_Picto__BarPlotPlotHandler.data,
      qt_meta_data_Picto__BarPlotPlotHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::BarPlotPlotHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::BarPlotPlotHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__BarPlotPlotHandler.stringdata0))
        return static_cast<void*>(const_cast< BarPlotPlotHandler*>(this));
    return BarBasePlotHandler::qt_metacast(_clname);
}

int Picto::BarPlotPlotHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BarBasePlotHandler::qt_metacall(_c, _id, _a);
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
