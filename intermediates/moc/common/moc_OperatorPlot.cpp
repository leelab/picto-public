/****************************************************************************
** Meta object code from reading C++ file 'OperatorPlot.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/operator/OperatorPlot.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OperatorPlot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__OperatorPlot_t {
    QByteArrayData data[14];
    char stringdata0[163];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__OperatorPlot_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__OperatorPlot_t qt_meta_stringdata_Picto__OperatorPlot = {
    {
QT_MOC_LITERAL(0, 0, 19), // "Picto::OperatorPlot"
QT_MOC_LITERAL(1, 20, 17), // "initializePlotSig"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 6), // "xTitle"
QT_MOC_LITERAL(4, 46, 6), // "yTitle"
QT_MOC_LITERAL(5, 53, 11), // "setTitleSig"
QT_MOC_LITERAL(6, 65, 5), // "title"
QT_MOC_LITERAL(7, 71, 22), // "connectToTaskConfigSig"
QT_MOC_LITERAL(8, 94, 26), // "QSharedPointer<TaskConfig>"
QT_MOC_LITERAL(9, 121, 11), // "pTaskConfig"
QT_MOC_LITERAL(10, 133, 10), // "exportPlot"
QT_MOC_LITERAL(11, 144, 4), // "type"
QT_MOC_LITERAL(12, 149, 4), // "size"
QT_MOC_LITERAL(13, 154, 8) // "fileName"

    },
    "Picto::OperatorPlot\0initializePlotSig\0"
    "\0xTitle\0yTitle\0setTitleSig\0title\0"
    "connectToTaskConfigSig\0"
    "QSharedPointer<TaskConfig>\0pTaskConfig\0"
    "exportPlot\0type\0size\0fileName"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__OperatorPlot[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x06 /* Public */,
       5,    1,   39,    2, 0x06 /* Public */,
       7,    1,   42,    2, 0x06 /* Public */,
      10,    3,   45,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QString,   11,   12,   13,

       0        // eod
};

void Picto::OperatorPlot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OperatorPlot *_t = static_cast<OperatorPlot *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->initializePlotSig((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: _t->setTitleSig((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->connectToTaskConfigSig((*reinterpret_cast< QSharedPointer<TaskConfig>(*)>(_a[1]))); break;
        case 3: _t->exportPlot((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<TaskConfig> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (OperatorPlot::*_t)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OperatorPlot::initializePlotSig)) {
                *result = 0;
            }
        }
        {
            typedef void (OperatorPlot::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OperatorPlot::setTitleSig)) {
                *result = 1;
            }
        }
        {
            typedef void (OperatorPlot::*_t)(QSharedPointer<TaskConfig> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OperatorPlot::connectToTaskConfigSig)) {
                *result = 2;
            }
        }
        {
            typedef void (OperatorPlot::*_t)(int , int , const QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OperatorPlot::exportPlot)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject Picto::OperatorPlot::staticMetaObject = {
    { &DataViewElement::staticMetaObject, qt_meta_stringdata_Picto__OperatorPlot.data,
      qt_meta_data_Picto__OperatorPlot,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::OperatorPlot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::OperatorPlot::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__OperatorPlot.stringdata0))
        return static_cast<void*>(const_cast< OperatorPlot*>(this));
    return DataViewElement::qt_metacast(_clname);
}

int Picto::OperatorPlot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DataViewElement::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Picto::OperatorPlot::initializePlotSig(const QString & _t1, const QString & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Picto::OperatorPlot::setTitleSig(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Picto::OperatorPlot::connectToTaskConfigSig(QSharedPointer<TaskConfig> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Picto::OperatorPlot::exportPlot(int _t1, int _t2, const QString _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
