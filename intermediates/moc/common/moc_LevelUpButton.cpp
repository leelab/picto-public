/****************************************************************************
** Meta object code from reading C++ file 'LevelUpButton.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/LevelUpButton.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LevelUpButton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_LevelUpButton_t {
    QByteArrayData data[8];
    char stringdata0[112];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LevelUpButton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LevelUpButton_t qt_meta_stringdata_LevelUpButton = {
    {
QT_MOC_LITERAL(0, 0, 13), // "LevelUpButton"
QT_MOC_LITERAL(1, 14, 15), // "searchRequested"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 13), // "SearchRequest"
QT_MOC_LITERAL(4, 45, 13), // "searchRequest"
QT_MOC_LITERAL(5, 59, 18), // "windowAssetChanged"
QT_MOC_LITERAL(6, 78, 21), // "QSharedPointer<Asset>"
QT_MOC_LITERAL(7, 100, 11) // "windowAsset"

    },
    "LevelUpButton\0searchRequested\0\0"
    "SearchRequest\0searchRequest\0"
    "windowAssetChanged\0QSharedPointer<Asset>\0"
    "windowAsset"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LevelUpButton[] = {

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
       1,    1,   24,    2, 0x08 /* Private */,
       5,    1,   27,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,

       0        // eod
};

void LevelUpButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LevelUpButton *_t = static_cast<LevelUpButton *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->searchRequested((*reinterpret_cast< SearchRequest(*)>(_a[1]))); break;
        case 1: _t->windowAssetChanged((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Asset> >(); break;
            }
            break;
        }
    }
}

const QMetaObject LevelUpButton::staticMetaObject = {
    { &QToolButton::staticMetaObject, qt_meta_stringdata_LevelUpButton.data,
      qt_meta_data_LevelUpButton,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *LevelUpButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LevelUpButton::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_LevelUpButton.stringdata0))
        return static_cast<void*>(const_cast< LevelUpButton*>(this));
    return QToolButton::qt_metacast(_clname);
}

int LevelUpButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolButton::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
