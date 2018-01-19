/****************************************************************************
** Meta object code from reading C++ file 'OperatorPlotHandler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/operator/OperatorPlotHandler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OperatorPlotHandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__OperatorPlotHandler_t {
    QByteArrayData data[15];
    char stringdata0[170];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__OperatorPlotHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__OperatorPlotHandler_t qt_meta_stringdata_Picto__OperatorPlotHandler = {
    {
QT_MOC_LITERAL(0, 0, 26), // "Picto::OperatorPlotHandler"
QT_MOC_LITERAL(1, 27, 14), // "initializePlot"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 6), // "xTitle"
QT_MOC_LITERAL(4, 50, 6), // "yTitle"
QT_MOC_LITERAL(5, 57, 8), // "setTitle"
QT_MOC_LITERAL(6, 66, 5), // "title"
QT_MOC_LITERAL(7, 72, 8), // "getTitle"
QT_MOC_LITERAL(8, 81, 19), // "connectToTaskConfig"
QT_MOC_LITERAL(9, 101, 26), // "QSharedPointer<TaskConfig>"
QT_MOC_LITERAL(10, 128, 11), // "pTaskConfig"
QT_MOC_LITERAL(11, 140, 10), // "exportPlot"
QT_MOC_LITERAL(12, 151, 4), // "type"
QT_MOC_LITERAL(13, 156, 4), // "size"
QT_MOC_LITERAL(14, 161, 8) // "fileName"

    },
    "Picto::OperatorPlotHandler\0initializePlot\0"
    "\0xTitle\0yTitle\0setTitle\0title\0getTitle\0"
    "connectToTaskConfig\0QSharedPointer<TaskConfig>\0"
    "pTaskConfig\0exportPlot\0type\0size\0"
    "fileName"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__OperatorPlotHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x0a /* Public */,
       5,    1,   44,    2, 0x0a /* Public */,
       7,    0,   47,    2, 0x0a /* Public */,
       8,    1,   48,    2, 0x0a /* Public */,
      11,    3,   51,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::QString,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QString,   12,   13,   14,

       0        // eod
};

void Picto::OperatorPlotHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OperatorPlotHandler *_t = static_cast<OperatorPlotHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->initializePlot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: _t->setTitle((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: { QString _r = _t->getTitle();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 3: _t->connectToTaskConfig((*reinterpret_cast< QSharedPointer<TaskConfig>(*)>(_a[1]))); break;
        case 4: _t->exportPlot((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject Picto::OperatorPlotHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Picto__OperatorPlotHandler.data,
      qt_meta_data_Picto__OperatorPlotHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::OperatorPlotHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::OperatorPlotHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__OperatorPlotHandler.stringdata0))
        return static_cast<void*>(const_cast< OperatorPlotHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int Picto::OperatorPlotHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
