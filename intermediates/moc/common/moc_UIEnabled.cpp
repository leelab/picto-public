/****************************************************************************
** Meta object code from reading C++ file 'UIEnabled.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/statemachine/UIEnabled.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'UIEnabled.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__UIEnabled_t {
    QByteArrayData data[3];
    char stringdata0[29];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__UIEnabled_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__UIEnabled_t qt_meta_stringdata_Picto__UIEnabled = {
    {
QT_MOC_LITERAL(0, 0, 16), // "Picto::UIEnabled"
QT_MOC_LITERAL(1, 17, 10), // "nameEdited"
QT_MOC_LITERAL(2, 28, 0) // ""

    },
    "Picto::UIEnabled\0nameEdited\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__UIEnabled[] = {

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
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

       0        // eod
};

void Picto::UIEnabled::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        UIEnabled *_t = static_cast<UIEnabled *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->nameEdited(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (UIEnabled::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&UIEnabled::nameEdited)) {
                *result = 0;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Picto::UIEnabled::staticMetaObject = {
    { &DataStore::staticMetaObject, qt_meta_stringdata_Picto__UIEnabled.data,
      qt_meta_data_Picto__UIEnabled,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::UIEnabled::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::UIEnabled::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__UIEnabled.stringdata0))
        return static_cast<void*>(const_cast< UIEnabled*>(this));
    return DataStore::qt_metacast(_clname);
}

int Picto::UIEnabled::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DataStore::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void Picto::UIEnabled::nameEdited()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
