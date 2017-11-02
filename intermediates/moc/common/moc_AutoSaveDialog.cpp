/****************************************************************************
** Meta object code from reading C++ file 'AutoSaveDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/AutoSaveDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AutoSaveDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__AutoSaveDialog_t {
    QByteArrayData data[12];
    char stringdata0[169];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__AutoSaveDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__AutoSaveDialog_t qt_meta_stringdata_Picto__AutoSaveDialog = {
    {
QT_MOC_LITERAL(0, 0, 21), // "Picto::AutoSaveDialog"
QT_MOC_LITERAL(1, 22, 17), // "deleteFileRequest"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 8), // "filePath"
QT_MOC_LITERAL(4, 50, 18), // "restoreFileRequest"
QT_MOC_LITERAL(5, 69, 14), // "updateContents"
QT_MOC_LITERAL(6, 84, 15), // "checkboxChanged"
QT_MOC_LITERAL(7, 100, 7), // "checked"
QT_MOC_LITERAL(8, 108, 15), // "deleteTriggered"
QT_MOC_LITERAL(9, 124, 11), // "buttonIndex"
QT_MOC_LITERAL(10, 136, 16), // "restoreTriggered"
QT_MOC_LITERAL(11, 153, 15) // "ignoreTriggered"

    },
    "Picto::AutoSaveDialog\0deleteFileRequest\0"
    "\0filePath\0restoreFileRequest\0"
    "updateContents\0checkboxChanged\0checked\0"
    "deleteTriggered\0buttonIndex\0"
    "restoreTriggered\0ignoreTriggered"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__AutoSaveDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       4,    1,   52,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   55,    2, 0x08 /* Private */,
       6,    1,   56,    2, 0x08 /* Private */,
       8,    1,   59,    2, 0x08 /* Private */,
      10,    1,   62,    2, 0x08 /* Private */,
      11,    1,   65,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void Picto::AutoSaveDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AutoSaveDialog *_t = static_cast<AutoSaveDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->deleteFileRequest((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->restoreFileRequest((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->updateContents(); break;
        case 3: _t->checkboxChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->deleteTriggered((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->restoreTriggered((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->ignoreTriggered((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AutoSaveDialog::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AutoSaveDialog::deleteFileRequest)) {
                *result = 0;
            }
        }
        {
            typedef void (AutoSaveDialog::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AutoSaveDialog::restoreFileRequest)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject Picto::AutoSaveDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Picto__AutoSaveDialog.data,
      qt_meta_data_Picto__AutoSaveDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::AutoSaveDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::AutoSaveDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__AutoSaveDialog.stringdata0))
        return static_cast<void*>(const_cast< AutoSaveDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int Picto::AutoSaveDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void Picto::AutoSaveDialog::deleteFileRequest(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Picto::AutoSaveDialog::restoreFileRequest(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
