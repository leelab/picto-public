/****************************************************************************
** Meta object code from reading C++ file 'PropertyFrame.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/PropertyFrame.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PropertyFrame.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PropertyFrame_t {
    QByteArrayData data[8];
    char stringdata0[98];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PropertyFrame_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PropertyFrame_t qt_meta_stringdata_PropertyFrame = {
    {
QT_MOC_LITERAL(0, 0, 13), // "PropertyFrame"
QT_MOC_LITERAL(1, 14, 21), // "parameterMessageReady"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 24), // "QSharedPointer<Property>"
QT_MOC_LITERAL(4, 62, 11), // "changedProp"
QT_MOC_LITERAL(5, 74, 14), // "propertyEdited"
QT_MOC_LITERAL(6, 89, 4), // "prop"
QT_MOC_LITERAL(7, 94, 3) // "val"

    },
    "PropertyFrame\0parameterMessageReady\0"
    "\0QSharedPointer<Property>\0changedProp\0"
    "propertyEdited\0prop\0val"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PropertyFrame[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    2,   27,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QVariant,    6,    7,

       0        // eod
};

void PropertyFrame::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PropertyFrame *_t = static_cast<PropertyFrame *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->parameterMessageReady((*reinterpret_cast< QSharedPointer<Property>(*)>(_a[1]))); break;
        case 1: _t->propertyEdited((*reinterpret_cast< QSharedPointer<Property>(*)>(_a[1])),(*reinterpret_cast< QVariant(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Property> >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Property> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PropertyFrame::*_t)(QSharedPointer<Property> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PropertyFrame::parameterMessageReady)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject PropertyFrame::staticMetaObject = {
    { &QScrollArea::staticMetaObject, qt_meta_stringdata_PropertyFrame.data,
      qt_meta_data_PropertyFrame,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PropertyFrame::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PropertyFrame::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PropertyFrame.stringdata0))
        return static_cast<void*>(const_cast< PropertyFrame*>(this));
    return QScrollArea::qt_metacast(_clname);
}

int PropertyFrame::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void PropertyFrame::parameterMessageReady(QSharedPointer<Property> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
