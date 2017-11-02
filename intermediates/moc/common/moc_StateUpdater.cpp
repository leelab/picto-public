/****************************************************************************
** Meta object code from reading C++ file 'StateUpdater.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/engine/StateUpdater.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StateUpdater.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__StateUpdater_t {
    QByteArrayData data[27];
    char stringdata0[312];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__StateUpdater_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__StateUpdater_t qt_meta_stringdata_Picto__StateUpdater = {
    {
QT_MOC_LITERAL(0, 0, 19), // "Picto::StateUpdater"
QT_MOC_LITERAL(1, 20, 11), // "startingRun"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 8), // "taskName"
QT_MOC_LITERAL(4, 42, 7), // "runName"
QT_MOC_LITERAL(5, 50, 9), // "endingRun"
QT_MOC_LITERAL(6, 60, 20), // "propertyValueChanged"
QT_MOC_LITERAL(7, 81, 6), // "dataId"
QT_MOC_LITERAL(8, 88, 6), // "propId"
QT_MOC_LITERAL(9, 95, 5), // "value"
QT_MOC_LITERAL(10, 101, 24), // "propertyInitValueChanged"
QT_MOC_LITERAL(11, 126, 19), // "transitionActivated"
QT_MOC_LITERAL(12, 146, 7), // "transId"
QT_MOC_LITERAL(13, 154, 15), // "remoteRunSignal"
QT_MOC_LITERAL(14, 170, 14), // "framePresented"
QT_MOC_LITERAL(15, 185, 4), // "time"
QT_MOC_LITERAL(16, 190, 14), // "rewardSupplied"
QT_MOC_LITERAL(17, 205, 8), // "duration"
QT_MOC_LITERAL(18, 214, 7), // "channel"
QT_MOC_LITERAL(19, 222, 13), // "signalChanged"
QT_MOC_LITERAL(20, 236, 4), // "name"
QT_MOC_LITERAL(21, 241, 12), // "subChanNames"
QT_MOC_LITERAL(22, 254, 14), // "QVector<float>"
QT_MOC_LITERAL(23, 269, 4), // "vals"
QT_MOC_LITERAL(24, 274, 16), // "disableRendering"
QT_MOC_LITERAL(25, 291, 7), // "disable"
QT_MOC_LITERAL(26, 299, 12) // "processQueue"

    },
    "Picto::StateUpdater\0startingRun\0\0"
    "taskName\0runName\0endingRun\0"
    "propertyValueChanged\0dataId\0propId\0"
    "value\0propertyInitValueChanged\0"
    "transitionActivated\0transId\0remoteRunSignal\0"
    "framePresented\0time\0rewardSupplied\0"
    "duration\0channel\0signalChanged\0name\0"
    "subChanNames\0QVector<float>\0vals\0"
    "disableRendering\0disable\0processQueue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__StateUpdater[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      13,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   79,    2, 0x06 /* Public */,
       5,    0,   84,    2, 0x06 /* Public */,
       6,    3,   85,    2, 0x06 /* Public */,
      10,    3,   92,    2, 0x06 /* Public */,
      11,    3,   99,    2, 0x06 /* Public */,
       6,    2,  106,    2, 0x06 /* Public */,
      10,    2,  111,    2, 0x06 /* Public */,
      11,    1,  116,    2, 0x06 /* Public */,
      14,    1,  119,    2, 0x06 /* Public */,
      16,    3,  122,    2, 0x06 /* Public */,
      19,    3,  129,    2, 0x06 /* Public */,
      24,    1,  136,    2, 0x06 /* Public */,
      26,    0,  139,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::ULongLong, QMetaType::Int, QMetaType::QString,    7,    8,    9,
    QMetaType::Void, QMetaType::ULongLong, QMetaType::Int, QMetaType::QString,    7,    8,    9,
    QMetaType::Void, QMetaType::ULongLong, QMetaType::Int, QMetaType::Bool,    7,   12,   13,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    8,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    8,    9,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Double,   15,
    QMetaType::Void, QMetaType::Double, QMetaType::Int, QMetaType::Int,   15,   17,   18,
    QMetaType::Void, QMetaType::QString, QMetaType::QStringList, 0x80000000 | 22,   20,   21,   23,
    QMetaType::Void, QMetaType::Bool,   25,
    QMetaType::Void,

       0        // eod
};

void Picto::StateUpdater::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        StateUpdater *_t = static_cast<StateUpdater *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->startingRun((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->endingRun(); break;
        case 2: _t->propertyValueChanged((*reinterpret_cast< qulonglong(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 3: _t->propertyInitValueChanged((*reinterpret_cast< qulonglong(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 4: _t->transitionActivated((*reinterpret_cast< qulonglong(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 5: _t->propertyValueChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 6: _t->propertyInitValueChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 7: _t->transitionActivated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->framePresented((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->rewardSupplied((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 10: _t->signalChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QStringList(*)>(_a[2])),(*reinterpret_cast< QVector<float>(*)>(_a[3]))); break;
        case 11: _t->disableRendering((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->processQueue(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<float> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (StateUpdater::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StateUpdater::startingRun)) {
                *result = 0;
            }
        }
        {
            typedef void (StateUpdater::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StateUpdater::endingRun)) {
                *result = 1;
            }
        }
        {
            typedef void (StateUpdater::*_t)(qulonglong , int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StateUpdater::propertyValueChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (StateUpdater::*_t)(qulonglong , int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StateUpdater::propertyInitValueChanged)) {
                *result = 3;
            }
        }
        {
            typedef void (StateUpdater::*_t)(qulonglong , int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StateUpdater::transitionActivated)) {
                *result = 4;
            }
        }
        {
            typedef void (StateUpdater::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StateUpdater::propertyValueChanged)) {
                *result = 5;
            }
        }
        {
            typedef void (StateUpdater::*_t)(int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StateUpdater::propertyInitValueChanged)) {
                *result = 6;
            }
        }
        {
            typedef void (StateUpdater::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StateUpdater::transitionActivated)) {
                *result = 7;
            }
        }
        {
            typedef void (StateUpdater::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StateUpdater::framePresented)) {
                *result = 8;
            }
        }
        {
            typedef void (StateUpdater::*_t)(double , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StateUpdater::rewardSupplied)) {
                *result = 9;
            }
        }
        {
            typedef void (StateUpdater::*_t)(QString , QStringList , QVector<float> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StateUpdater::signalChanged)) {
                *result = 10;
            }
        }
        {
            typedef void (StateUpdater::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StateUpdater::disableRendering)) {
                *result = 11;
            }
        }
        {
            typedef void (StateUpdater::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StateUpdater::processQueue)) {
                *result = 12;
            }
        }
    }
}

const QMetaObject Picto::StateUpdater::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Picto__StateUpdater.data,
      qt_meta_data_Picto__StateUpdater,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::StateUpdater::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::StateUpdater::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__StateUpdater.stringdata0))
        return static_cast<void*>(const_cast< StateUpdater*>(this));
    return QObject::qt_metacast(_clname);
}

int Picto::StateUpdater::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void Picto::StateUpdater::startingRun(QString _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Picto::StateUpdater::endingRun()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void Picto::StateUpdater::propertyValueChanged(qulonglong _t1, int _t2, QString _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Picto::StateUpdater::propertyInitValueChanged(qulonglong _t1, int _t2, QString _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Picto::StateUpdater::transitionActivated(qulonglong _t1, int _t2, bool _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Picto::StateUpdater::propertyValueChanged(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Picto::StateUpdater::propertyInitValueChanged(int _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Picto::StateUpdater::transitionActivated(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Picto::StateUpdater::framePresented(double _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void Picto::StateUpdater::rewardSupplied(double _t1, int _t2, int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void Picto::StateUpdater::signalChanged(QString _t1, QStringList _t2, QVector<float> _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void Picto::StateUpdater::disableRendering(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void Picto::StateUpdater::processQueue()
{
    QMetaObject::activate(this, &staticMetaObject, 12, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
