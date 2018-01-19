/****************************************************************************
** Meta object code from reading C++ file 'AboutElementBox.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/AboutElementBox.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AboutElementBox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AboutElementBox_t {
    QByteArrayData data[11];
    char stringdata0[151];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AboutElementBox_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AboutElementBox_t qt_meta_stringdata_AboutElementBox = {
    {
QT_MOC_LITERAL(0, 0, 15), // "AboutElementBox"
QT_MOC_LITERAL(1, 16, 13), // "assetSelected"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 21), // "QSharedPointer<Asset>"
QT_MOC_LITERAL(4, 53, 5), // "asset"
QT_MOC_LITERAL(5, 59, 24), // "elementOfInterestChanged"
QT_MOC_LITERAL(6, 84, 9), // "className"
QT_MOC_LITERAL(7, 94, 12), // "friendlyName"
QT_MOC_LITERAL(8, 107, 15), // "searchRequested"
QT_MOC_LITERAL(9, 123, 13), // "SearchRequest"
QT_MOC_LITERAL(10, 137, 13) // "searchRequest"

    },
    "AboutElementBox\0assetSelected\0\0"
    "QSharedPointer<Asset>\0asset\0"
    "elementOfInterestChanged\0className\0"
    "friendlyName\0searchRequested\0SearchRequest\0"
    "searchRequest"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AboutElementBox[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x0a /* Public */,
       5,    2,   32,    2, 0x0a /* Public */,
       8,    1,   37,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,

       0        // eod
};

void AboutElementBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AboutElementBox *_t = static_cast<AboutElementBox *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->assetSelected((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
        case 1: _t->elementOfInterestChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 2: _t->searchRequested((*reinterpret_cast< SearchRequest(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Asset> >(); break;
            }
            break;
        }
    }
}

const QMetaObject AboutElementBox::staticMetaObject = {
    { &SearchableTextEdit::staticMetaObject, qt_meta_stringdata_AboutElementBox.data,
      qt_meta_data_AboutElementBox,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *AboutElementBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AboutElementBox::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_AboutElementBox.stringdata0))
        return static_cast<void*>(const_cast< AboutElementBox*>(this));
    return SearchableTextEdit::qt_metacast(_clname);
}

int AboutElementBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SearchableTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
