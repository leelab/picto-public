/****************************************************************************
** Meta object code from reading C++ file 'SamplingBarBasePlotHandler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/operator/SamplingBarBasePlotHandler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SamplingBarBasePlotHandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__SamplingBarBasePlotHandler_t {
    QByteArrayData data[10];
    char stringdata0[159];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__SamplingBarBasePlotHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__SamplingBarBasePlotHandler_t qt_meta_stringdata_Picto__SamplingBarBasePlotHandler = {
    {
QT_MOC_LITERAL(0, 0, 33), // "Picto::SamplingBarBasePlotHan..."
QT_MOC_LITERAL(1, 34, 18), // "initializeSampling"
QT_MOC_LITERAL(2, 53, 0), // ""
QT_MOC_LITERAL(3, 54, 15), // "setErrorSamples"
QT_MOC_LITERAL(4, 70, 7), // "dataSet"
QT_MOC_LITERAL(5, 78, 26), // "QVector<QwtIntervalSample>"
QT_MOC_LITERAL(6, 105, 14), // "qvErrorSamples"
QT_MOC_LITERAL(7, 120, 19), // "setErrorBarsVisible"
QT_MOC_LITERAL(8, 140, 8), // "bVisible"
QT_MOC_LITERAL(9, 149, 9) // "resetSlot"

    },
    "Picto::SamplingBarBasePlotHandler\0"
    "initializeSampling\0\0setErrorSamples\0"
    "dataSet\0QVector<QwtIntervalSample>\0"
    "qvErrorSamples\0setErrorBarsVisible\0"
    "bVisible\0resetSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__SamplingBarBasePlotHandler[] = {

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
       1,    0,   34,    2, 0x0a /* Public */,
       3,    2,   35,    2, 0x0a /* Public */,
       7,    1,   40,    2, 0x0a /* Public */,
       9,    0,   43,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 5,    4,    6,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void,

       0        // eod
};

void Picto::SamplingBarBasePlotHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SamplingBarBasePlotHandler *_t = static_cast<SamplingBarBasePlotHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->initializeSampling(); break;
        case 1: _t->setErrorSamples((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVector<QwtIntervalSample>(*)>(_a[2]))); break;
        case 2: _t->setErrorBarsVisible((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->resetSlot(); break;
        default: ;
        }
    }
}

const QMetaObject Picto::SamplingBarBasePlotHandler::staticMetaObject = {
    { &BarBasePlotHandler::staticMetaObject, qt_meta_stringdata_Picto__SamplingBarBasePlotHandler.data,
      qt_meta_data_Picto__SamplingBarBasePlotHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::SamplingBarBasePlotHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::SamplingBarBasePlotHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__SamplingBarBasePlotHandler.stringdata0))
        return static_cast<void*>(const_cast< SamplingBarBasePlotHandler*>(this));
    return BarBasePlotHandler::qt_metacast(_clname);
}

int Picto::SamplingBarBasePlotHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BarBasePlotHandler::qt_metacall(_c, _id, _a);
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
