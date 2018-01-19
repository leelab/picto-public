/****************************************************************************
** Meta object code from reading C++ file 'RemoteStateXMLHandler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/workstation/RemoteViewer/RemoteStateXMLHandler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RemoteStateXMLHandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__RemoteStateXMLHandler_t {
    QByteArrayData data[1];
    char stringdata0[29];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__RemoteStateXMLHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__RemoteStateXMLHandler_t qt_meta_stringdata_Picto__RemoteStateXMLHandler = {
    {
QT_MOC_LITERAL(0, 0, 28) // "Picto::RemoteStateXMLHandler"

    },
    "Picto::RemoteStateXMLHandler"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__RemoteStateXMLHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void Picto::RemoteStateXMLHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Picto::RemoteStateXMLHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Picto__RemoteStateXMLHandler.data,
      qt_meta_data_Picto__RemoteStateXMLHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::RemoteStateXMLHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::RemoteStateXMLHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__RemoteStateXMLHandler.stringdata0))
        return static_cast<void*>(const_cast< RemoteStateXMLHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int Picto::RemoteStateXMLHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_Picto__PropertyDataUnitHandler_t {
    QByteArrayData data[7];
    char stringdata0[98];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__PropertyDataUnitHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__PropertyDataUnitHandler_t qt_meta_stringdata_Picto__PropertyDataUnitHandler = {
    {
QT_MOC_LITERAL(0, 0, 30), // "Picto::PropertyDataUnitHandler"
QT_MOC_LITERAL(1, 31, 20), // "propertyValueChanged"
QT_MOC_LITERAL(2, 52, 0), // ""
QT_MOC_LITERAL(3, 53, 6), // "dataId"
QT_MOC_LITERAL(4, 60, 6), // "propId"
QT_MOC_LITERAL(5, 67, 5), // "value"
QT_MOC_LITERAL(6, 73, 24) // "propertyInitValueChanged"

    },
    "Picto::PropertyDataUnitHandler\0"
    "propertyValueChanged\0\0dataId\0propId\0"
    "value\0propertyInitValueChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__PropertyDataUnitHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   24,    2, 0x06 /* Public */,
       6,    3,   31,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::ULongLong, QMetaType::Int, QMetaType::QString,    3,    4,    5,
    QMetaType::Void, QMetaType::ULongLong, QMetaType::Int, QMetaType::QString,    3,    4,    5,

       0        // eod
};

void Picto::PropertyDataUnitHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PropertyDataUnitHandler *_t = static_cast<PropertyDataUnitHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->propertyValueChanged((*reinterpret_cast< qulonglong(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 1: _t->propertyInitValueChanged((*reinterpret_cast< qulonglong(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PropertyDataUnitHandler::*_t)(qulonglong , int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PropertyDataUnitHandler::propertyValueChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (PropertyDataUnitHandler::*_t)(qulonglong , int , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PropertyDataUnitHandler::propertyInitValueChanged)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject Picto::PropertyDataUnitHandler::staticMetaObject = {
    { &RemoteStateXMLHandler::staticMetaObject, qt_meta_stringdata_Picto__PropertyDataUnitHandler.data,
      qt_meta_data_Picto__PropertyDataUnitHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::PropertyDataUnitHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::PropertyDataUnitHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__PropertyDataUnitHandler.stringdata0))
        return static_cast<void*>(const_cast< PropertyDataUnitHandler*>(this));
    return RemoteStateXMLHandler::qt_metacast(_clname);
}

int Picto::PropertyDataUnitHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = RemoteStateXMLHandler::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void Picto::PropertyDataUnitHandler::propertyValueChanged(qulonglong _t1, int _t2, QString _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Picto::PropertyDataUnitHandler::propertyInitValueChanged(qulonglong _t1, int _t2, QString _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_Picto__BehavioralDataUnitHandler_t {
    QByteArrayData data[7];
    char stringdata0[86];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__BehavioralDataUnitHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__BehavioralDataUnitHandler_t qt_meta_stringdata_Picto__BehavioralDataUnitHandler = {
    {
QT_MOC_LITERAL(0, 0, 32), // "Picto::BehavioralDataUnitHandler"
QT_MOC_LITERAL(1, 33, 13), // "signalChanged"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 4), // "name"
QT_MOC_LITERAL(4, 53, 12), // "subChanNames"
QT_MOC_LITERAL(5, 66, 14), // "QVector<float>"
QT_MOC_LITERAL(6, 81, 4) // "vals"

    },
    "Picto::BehavioralDataUnitHandler\0"
    "signalChanged\0\0name\0subChanNames\0"
    "QVector<float>\0vals"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__BehavioralDataUnitHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QStringList, 0x80000000 | 5,    3,    4,    6,

       0        // eod
};

void Picto::BehavioralDataUnitHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BehavioralDataUnitHandler *_t = static_cast<BehavioralDataUnitHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->signalChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QStringList(*)>(_a[2])),(*reinterpret_cast< QVector<float>(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
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
            typedef void (BehavioralDataUnitHandler::*_t)(QString , QStringList , QVector<float> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BehavioralDataUnitHandler::signalChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject Picto::BehavioralDataUnitHandler::staticMetaObject = {
    { &RemoteStateXMLHandler::staticMetaObject, qt_meta_stringdata_Picto__BehavioralDataUnitHandler.data,
      qt_meta_data_Picto__BehavioralDataUnitHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::BehavioralDataUnitHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::BehavioralDataUnitHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__BehavioralDataUnitHandler.stringdata0))
        return static_cast<void*>(const_cast< BehavioralDataUnitHandler*>(this));
    return RemoteStateXMLHandler::qt_metacast(_clname);
}

int Picto::BehavioralDataUnitHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = RemoteStateXMLHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Picto::BehavioralDataUnitHandler::signalChanged(QString _t1, QStringList _t2, QVector<float> _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_Picto__InputDataUnitHandler_t {
    QByteArrayData data[7];
    char stringdata0[81];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__InputDataUnitHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__InputDataUnitHandler_t qt_meta_stringdata_Picto__InputDataUnitHandler = {
    {
QT_MOC_LITERAL(0, 0, 27), // "Picto::InputDataUnitHandler"
QT_MOC_LITERAL(1, 28, 13), // "signalChanged"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 4), // "name"
QT_MOC_LITERAL(4, 48, 12), // "subChanNames"
QT_MOC_LITERAL(5, 61, 14), // "QVector<float>"
QT_MOC_LITERAL(6, 76, 4) // "vals"

    },
    "Picto::InputDataUnitHandler\0signalChanged\0"
    "\0name\0subChanNames\0QVector<float>\0"
    "vals"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__InputDataUnitHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QStringList, 0x80000000 | 5,    3,    4,    6,

       0        // eod
};

void Picto::InputDataUnitHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        InputDataUnitHandler *_t = static_cast<InputDataUnitHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->signalChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QStringList(*)>(_a[2])),(*reinterpret_cast< QVector<float>(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
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
            typedef void (InputDataUnitHandler::*_t)(QString , QStringList , QVector<float> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&InputDataUnitHandler::signalChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject Picto::InputDataUnitHandler::staticMetaObject = {
    { &RemoteStateXMLHandler::staticMetaObject, qt_meta_stringdata_Picto__InputDataUnitHandler.data,
      qt_meta_data_Picto__InputDataUnitHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::InputDataUnitHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::InputDataUnitHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__InputDataUnitHandler.stringdata0))
        return static_cast<void*>(const_cast< InputDataUnitHandler*>(this));
    return RemoteStateXMLHandler::qt_metacast(_clname);
}

int Picto::InputDataUnitHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = RemoteStateXMLHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Picto::InputDataUnitHandler::signalChanged(QString _t1, QStringList _t2, QVector<float> _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_Picto__StateDataUnitHandler_t {
    QByteArrayData data[7];
    char stringdata0[102];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__StateDataUnitHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__StateDataUnitHandler_t qt_meta_stringdata_Picto__StateDataUnitHandler = {
    {
QT_MOC_LITERAL(0, 0, 27), // "Picto::StateDataUnitHandler"
QT_MOC_LITERAL(1, 28, 19), // "transitionActivated"
QT_MOC_LITERAL(2, 48, 0), // ""
QT_MOC_LITERAL(3, 49, 6), // "dataId"
QT_MOC_LITERAL(4, 56, 7), // "transId"
QT_MOC_LITERAL(5, 64, 15), // "remoteRunSignal"
QT_MOC_LITERAL(6, 80, 21) // "prepareToProcessQueue"

    },
    "Picto::StateDataUnitHandler\0"
    "transitionActivated\0\0dataId\0transId\0"
    "remoteRunSignal\0prepareToProcessQueue"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__StateDataUnitHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   24,    2, 0x06 /* Public */,
       6,    0,   31,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::ULongLong, QMetaType::Int, QMetaType::Bool,    3,    4,    5,
    QMetaType::Void,

       0        // eod
};

void Picto::StateDataUnitHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        StateDataUnitHandler *_t = static_cast<StateDataUnitHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->transitionActivated((*reinterpret_cast< qulonglong(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->prepareToProcessQueue(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (StateDataUnitHandler::*_t)(qulonglong , int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StateDataUnitHandler::transitionActivated)) {
                *result = 0;
            }
        }
        {
            typedef void (StateDataUnitHandler::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&StateDataUnitHandler::prepareToProcessQueue)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject Picto::StateDataUnitHandler::staticMetaObject = {
    { &RemoteStateXMLHandler::staticMetaObject, qt_meta_stringdata_Picto__StateDataUnitHandler.data,
      qt_meta_data_Picto__StateDataUnitHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::StateDataUnitHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::StateDataUnitHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__StateDataUnitHandler.stringdata0))
        return static_cast<void*>(const_cast< StateDataUnitHandler*>(this));
    return RemoteStateXMLHandler::qt_metacast(_clname);
}

int Picto::StateDataUnitHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = RemoteStateXMLHandler::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void Picto::StateDataUnitHandler::transitionActivated(qulonglong _t1, int _t2, bool _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Picto::StateDataUnitHandler::prepareToProcessQueue()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
struct qt_meta_stringdata_Picto__FrameDataUnitHandler_t {
    QByteArrayData data[5];
    char stringdata0[68];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__FrameDataUnitHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__FrameDataUnitHandler_t qt_meta_stringdata_Picto__FrameDataUnitHandler = {
    {
QT_MOC_LITERAL(0, 0, 27), // "Picto::FrameDataUnitHandler"
QT_MOC_LITERAL(1, 28, 14), // "framePresented"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 4), // "time"
QT_MOC_LITERAL(4, 49, 18) // "updateCurrUnitTime"

    },
    "Picto::FrameDataUnitHandler\0framePresented\0"
    "\0time\0updateCurrUnitTime"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__FrameDataUnitHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,
       4,    1,   27,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void Picto::FrameDataUnitHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FrameDataUnitHandler *_t = static_cast<FrameDataUnitHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->framePresented((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->updateCurrUnitTime((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (FrameDataUnitHandler::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FrameDataUnitHandler::framePresented)) {
                *result = 0;
            }
        }
        {
            typedef void (FrameDataUnitHandler::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FrameDataUnitHandler::updateCurrUnitTime)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject Picto::FrameDataUnitHandler::staticMetaObject = {
    { &RemoteStateXMLHandler::staticMetaObject, qt_meta_stringdata_Picto__FrameDataUnitHandler.data,
      qt_meta_data_Picto__FrameDataUnitHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::FrameDataUnitHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::FrameDataUnitHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__FrameDataUnitHandler.stringdata0))
        return static_cast<void*>(const_cast< FrameDataUnitHandler*>(this));
    return RemoteStateXMLHandler::qt_metacast(_clname);
}

int Picto::FrameDataUnitHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = RemoteStateXMLHandler::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void Picto::FrameDataUnitHandler::framePresented(double _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Picto::FrameDataUnitHandler::updateCurrUnitTime(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_Picto__RewardDataUnitHandler_t {
    QByteArrayData data[7];
    char stringdata0[86];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__RewardDataUnitHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__RewardDataUnitHandler_t qt_meta_stringdata_Picto__RewardDataUnitHandler = {
    {
QT_MOC_LITERAL(0, 0, 28), // "Picto::RewardDataUnitHandler"
QT_MOC_LITERAL(1, 29, 14), // "rewardSupplied"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 4), // "time"
QT_MOC_LITERAL(4, 50, 8), // "duration"
QT_MOC_LITERAL(5, 59, 7), // "channel"
QT_MOC_LITERAL(6, 67, 18) // "updateCurrUnitTime"

    },
    "Picto::RewardDataUnitHandler\0"
    "rewardSupplied\0\0time\0duration\0channel\0"
    "updateCurrUnitTime"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__RewardDataUnitHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   24,    2, 0x06 /* Public */,
       6,    1,   31,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double, QMetaType::Int, QMetaType::Int,    3,    4,    5,
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void Picto::RewardDataUnitHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RewardDataUnitHandler *_t = static_cast<RewardDataUnitHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->rewardSupplied((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 1: _t->updateCurrUnitTime((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RewardDataUnitHandler::*_t)(double , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RewardDataUnitHandler::rewardSupplied)) {
                *result = 0;
            }
        }
        {
            typedef void (RewardDataUnitHandler::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RewardDataUnitHandler::updateCurrUnitTime)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject Picto::RewardDataUnitHandler::staticMetaObject = {
    { &RemoteStateXMLHandler::staticMetaObject, qt_meta_stringdata_Picto__RewardDataUnitHandler.data,
      qt_meta_data_Picto__RewardDataUnitHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::RewardDataUnitHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::RewardDataUnitHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__RewardDataUnitHandler.stringdata0))
        return static_cast<void*>(const_cast< RewardDataUnitHandler*>(this));
    return RemoteStateXMLHandler::qt_metacast(_clname);
}

int Picto::RewardDataUnitHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = RemoteStateXMLHandler::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void Picto::RewardDataUnitHandler::rewardSupplied(double _t1, int _t2, int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Picto::RewardDataUnitHandler::updateCurrUnitTime(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
