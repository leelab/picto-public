/****************************************************************************
** Meta object code from reading C++ file 'TaskConfig.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/storage/TaskConfig.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TaskConfig.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__TaskConfig_t {
    QByteArrayData data[25];
    char stringdata0[346];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__TaskConfig_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__TaskConfig_t qt_meta_stringdata_Picto__TaskConfig = {
    {
QT_MOC_LITERAL(0, 0, 17), // "Picto::TaskConfig"
QT_MOC_LITERAL(1, 18, 16), // "widgetAddedToMap"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 8), // "QWidget*"
QT_MOC_LITERAL(4, 45, 7), // "pWidget"
QT_MOC_LITERAL(5, 53, 20), // "widgetRemovedFromMap"
QT_MOC_LITERAL(6, 74, 20), // "plotHandlerRequested"
QT_MOC_LITERAL(7, 95, 8), // "plotPath"
QT_MOC_LITERAL(8, 104, 23), // "requestCachePlotHandler"
QT_MOC_LITERAL(9, 128, 35), // "QSharedPointer<OperatorPlotHa..."
QT_MOC_LITERAL(10, 164, 7), // "handler"
QT_MOC_LITERAL(11, 172, 24), // "requestClearPlotHandlers"
QT_MOC_LITERAL(12, 197, 10), // "spikeAdded"
QT_MOC_LITERAL(13, 208, 7), // "channel"
QT_MOC_LITERAL(14, 216, 4), // "unit"
QT_MOC_LITERAL(15, 221, 4), // "time"
QT_MOC_LITERAL(16, 226, 18), // "receivePlotHandler"
QT_MOC_LITERAL(17, 245, 28), // "managerConnectionEstablished"
QT_MOC_LITERAL(18, 274, 9), // "connected"
QT_MOC_LITERAL(19, 284, 9), // "alignPlot"
QT_MOC_LITERAL(20, 294, 7), // "alignID"
QT_MOC_LITERAL(21, 302, 10), // "eventAdded"
QT_MOC_LITERAL(22, 313, 7), // "eventID"
QT_MOC_LITERAL(23, 321, 15), // "scriptContAdded"
QT_MOC_LITERAL(24, 337, 8) // "scriptID"

    },
    "Picto::TaskConfig\0widgetAddedToMap\0\0"
    "QWidget*\0pWidget\0widgetRemovedFromMap\0"
    "plotHandlerRequested\0plotPath\0"
    "requestCachePlotHandler\0"
    "QSharedPointer<OperatorPlotHandler>\0"
    "handler\0requestClearPlotHandlers\0"
    "spikeAdded\0channel\0unit\0time\0"
    "receivePlotHandler\0managerConnectionEstablished\0"
    "connected\0alignPlot\0alignID\0eventAdded\0"
    "eventID\0scriptContAdded\0scriptID"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__TaskConfig[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06 /* Public */,
       5,    1,   72,    2, 0x06 /* Public */,
       6,    1,   75,    2, 0x06 /* Public */,
       8,    2,   78,    2, 0x06 /* Public */,
      11,    0,   83,    2, 0x06 /* Public */,
      12,    3,   84,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      16,    2,   91,    2, 0x0a /* Public */,
      17,    1,   96,    2, 0x0a /* Public */,
      19,    1,   99,    2, 0x0a /* Public */,
      21,    1,  102,    2, 0x0a /* Public */,
      23,    1,  105,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, 0x80000000 | 9, QMetaType::QString,   10,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Double,   13,   14,   15,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 9, QMetaType::QString,   10,    7,
    QMetaType::Void, QMetaType::Bool,   18,
    QMetaType::Void, QMetaType::Int,   20,
    QMetaType::Void, QMetaType::Int,   22,
    QMetaType::Void, QMetaType::Int,   24,

       0        // eod
};

void Picto::TaskConfig::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TaskConfig *_t = static_cast<TaskConfig *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->widgetAddedToMap((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 1: _t->widgetRemovedFromMap((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 2: _t->plotHandlerRequested((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->requestCachePlotHandler((*reinterpret_cast< QSharedPointer<OperatorPlotHandler>(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: _t->requestClearPlotHandlers(); break;
        case 5: _t->spikeAdded((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 6: _t->receivePlotHandler((*reinterpret_cast< QSharedPointer<OperatorPlotHandler>(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 7: _t->managerConnectionEstablished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->alignPlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->eventAdded((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->scriptContAdded((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<OperatorPlotHandler> >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<OperatorPlotHandler> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (TaskConfig::*_t)(QWidget * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TaskConfig::widgetAddedToMap)) {
                *result = 0;
            }
        }
        {
            typedef void (TaskConfig::*_t)(QWidget * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TaskConfig::widgetRemovedFromMap)) {
                *result = 1;
            }
        }
        {
            typedef void (TaskConfig::*_t)(const QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TaskConfig::plotHandlerRequested)) {
                *result = 2;
            }
        }
        {
            typedef void (TaskConfig::*_t)(QSharedPointer<OperatorPlotHandler> , const QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TaskConfig::requestCachePlotHandler)) {
                *result = 3;
            }
        }
        {
            typedef void (TaskConfig::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TaskConfig::requestClearPlotHandlers)) {
                *result = 4;
            }
        }
        {
            typedef void (TaskConfig::*_t)(int , int , double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TaskConfig::spikeAdded)) {
                *result = 5;
            }
        }
    }
}

const QMetaObject Picto::TaskConfig::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Picto__TaskConfig.data,
      qt_meta_data_Picto__TaskConfig,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::TaskConfig::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::TaskConfig::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__TaskConfig.stringdata0))
        return static_cast<void*>(const_cast< TaskConfig*>(this));
    return QObject::qt_metacast(_clname);
}

int Picto::TaskConfig::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void Picto::TaskConfig::widgetAddedToMap(QWidget * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Picto::TaskConfig::widgetRemovedFromMap(QWidget * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Picto::TaskConfig::plotHandlerRequested(const QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Picto::TaskConfig::requestCachePlotHandler(QSharedPointer<OperatorPlotHandler> _t1, const QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Picto::TaskConfig::requestClearPlotHandlers()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}

// SIGNAL 5
void Picto::TaskConfig::spikeAdded(int _t1, int _t2, double _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
