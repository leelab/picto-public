/****************************************************************************
** Meta object code from reading C++ file 'RasterBase.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/operator/RasterBase.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RasterBase.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__RasterBase_t {
    QByteArrayData data[27];
    char stringdata0[250];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__RasterBase_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__RasterBase_t qt_meta_stringdata_Picto__RasterBase = {
    {
QT_MOC_LITERAL(0, 0, 17), // "Picto::RasterBase"
QT_MOC_LITERAL(1, 18, 17), // "initializePlotSig"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 14), // "bDisplayLegend"
QT_MOC_LITERAL(4, 52, 8), // "barColor"
QT_MOC_LITERAL(5, 61, 11), // "canvasColor"
QT_MOC_LITERAL(6, 73, 16), // "updateColumnsSig"
QT_MOC_LITERAL(7, 90, 7), // "dataSet"
QT_MOC_LITERAL(8, 98, 5), // "color"
QT_MOC_LITERAL(9, 104, 12), // "setPointsSig"
QT_MOC_LITERAL(10, 117, 16), // "QVector<QPointF>"
QT_MOC_LITERAL(11, 134, 8), // "qvPoints"
QT_MOC_LITERAL(12, 143, 12), // "scaleAxisSig"
QT_MOC_LITERAL(13, 156, 8), // "dBinSize"
QT_MOC_LITERAL(14, 165, 16), // "handleXLabelsSig"
QT_MOC_LITERAL(15, 182, 10), // "callReplot"
QT_MOC_LITERAL(16, 193, 8), // "resetSig"
QT_MOC_LITERAL(17, 202, 9), // "clearPlot"
QT_MOC_LITERAL(18, 212, 8), // "setColor"
QT_MOC_LITERAL(19, 221, 1), // "r"
QT_MOC_LITERAL(20, 223, 1), // "g"
QT_MOC_LITERAL(21, 225, 1), // "b"
QT_MOC_LITERAL(22, 227, 1), // "a"
QT_MOC_LITERAL(23, 229, 3), // "red"
QT_MOC_LITERAL(24, 233, 5), // "green"
QT_MOC_LITERAL(25, 239, 4), // "blue"
QT_MOC_LITERAL(26, 244, 5) // "alpha"

    },
    "Picto::RasterBase\0initializePlotSig\0"
    "\0bDisplayLegend\0barColor\0canvasColor\0"
    "updateColumnsSig\0dataSet\0color\0"
    "setPointsSig\0QVector<QPointF>\0qvPoints\0"
    "scaleAxisSig\0dBinSize\0handleXLabelsSig\0"
    "callReplot\0resetSig\0clearPlot\0setColor\0"
    "r\0g\0b\0a\0red\0green\0blue\0alpha"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__RasterBase[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       5,  110, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   64,    2, 0x06 /* Public */,
       6,    2,   71,    2, 0x06 /* Public */,
       9,    2,   76,    2, 0x06 /* Public */,
      12,    1,   81,    2, 0x06 /* Public */,
      14,    2,   84,    2, 0x06 /* Public */,
      15,    1,   89,    2, 0x06 /* Public */,
      16,    0,   92,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      17,    0,   93,    2, 0x0a /* Public */,
      18,    4,   94,    2, 0x0a /* Public */,
      18,    3,  103,    2, 0x2a /* Public | MethodCloned */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::QColor, QMetaType::QColor,    3,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::QColor,    7,    8,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 10,    7,   11,
    QMetaType::Void, QMetaType::Double,   13,
    QMetaType::Void, QMetaType::Long, QMetaType::Long,    2,    2,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,   19,   20,   21,   22,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   19,   20,   21,

 // properties: name, type, flags
      23, QMetaType::Int, 0x00095103,
      24, QMetaType::Int, 0x00095103,
      25, QMetaType::Int, 0x00095103,
      26, QMetaType::Int, 0x00095103,
       8, QMetaType::QVariant, 0x00095103,

       0        // eod
};

void Picto::RasterBase::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RasterBase *_t = static_cast<RasterBase *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->initializePlotSig((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2])),(*reinterpret_cast< const QColor(*)>(_a[3]))); break;
        case 1: _t->updateColumnsSig((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 2: _t->setPointsSig((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVector<QPointF>(*)>(_a[2]))); break;
        case 3: _t->scaleAxisSig((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->handleXLabelsSig((*reinterpret_cast< long(*)>(_a[1])),(*reinterpret_cast< long(*)>(_a[2]))); break;
        case 5: _t->callReplot((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->resetSig(); break;
        case 7: _t->clearPlot(); break;
        case 8: _t->setColor((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 9: _t->setColor((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<QPointF> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RasterBase::*_t)(bool , const QColor & , const QColor & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RasterBase::initializePlotSig)) {
                *result = 0;
            }
        }
        {
            typedef void (RasterBase::*_t)(const QString & , const QColor & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RasterBase::updateColumnsSig)) {
                *result = 1;
            }
        }
        {
            typedef void (RasterBase::*_t)(const QString & , const QVector<QPointF> & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RasterBase::setPointsSig)) {
                *result = 2;
            }
        }
        {
            typedef void (RasterBase::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RasterBase::scaleAxisSig)) {
                *result = 3;
            }
        }
        {
            typedef void (RasterBase::*_t)(long , long );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RasterBase::handleXLabelsSig)) {
                *result = 4;
            }
        }
        {
            typedef void (RasterBase::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RasterBase::callReplot)) {
                *result = 5;
            }
        }
        {
            typedef void (RasterBase::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RasterBase::resetSig)) {
                *result = 6;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        RasterBase *_t = static_cast<RasterBase *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getRed(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->getGreen(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->getBlue(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->getAlpha(); break;
        case 4: *reinterpret_cast< QVariant*>(_v) = _t->getColor(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        RasterBase *_t = static_cast<RasterBase *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setRed(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setGreen(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setBlue(*reinterpret_cast< int*>(_v)); break;
        case 3: _t->setAlpha(*reinterpret_cast< int*>(_v)); break;
        case 4: _t->setColor(*reinterpret_cast< QVariant*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject Picto::RasterBase::staticMetaObject = {
    { &OperatorPlot::staticMetaObject, qt_meta_stringdata_Picto__RasterBase.data,
      qt_meta_data_Picto__RasterBase,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::RasterBase::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::RasterBase::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__RasterBase.stringdata0))
        return static_cast<void*>(const_cast< RasterBase*>(this));
    return OperatorPlot::qt_metacast(_clname);
}

int Picto::RasterBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = OperatorPlot::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Picto::RasterBase::initializePlotSig(bool _t1, const QColor & _t2, const QColor & _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Picto::RasterBase::updateColumnsSig(const QString & _t1, const QColor & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Picto::RasterBase::setPointsSig(const QString & _t1, const QVector<QPointF> & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Picto::RasterBase::scaleAxisSig(double _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Picto::RasterBase::handleXLabelsSig(long _t1, long _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Picto::RasterBase::callReplot(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Picto::RasterBase::resetSig()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
