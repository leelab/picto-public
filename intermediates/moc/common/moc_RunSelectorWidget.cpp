/****************************************************************************
** Meta object code from reading C++ file 'RunSelectorWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/RunSelectorWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RunSelectorWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__RunSelectorWidget_t {
    QByteArrayData data[8];
    char stringdata0[109];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__RunSelectorWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__RunSelectorWidget_t qt_meta_stringdata_Picto__RunSelectorWidget = {
    {
QT_MOC_LITERAL(0, 0, 24), // "Picto::RunSelectorWidget"
QT_MOC_LITERAL(1, 25, 19), // "runSelectionChanged"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 13), // "buttonClicked"
QT_MOC_LITERAL(4, 60, 11), // "buttonIndex"
QT_MOC_LITERAL(5, 72, 9), // "selectAll"
QT_MOC_LITERAL(6, 82, 11), // "selectSaved"
QT_MOC_LITERAL(7, 94, 14) // "clearSelection"

    },
    "Picto::RunSelectorWidget\0runSelectionChanged\0"
    "\0buttonClicked\0buttonIndex\0selectAll\0"
    "selectSaved\0clearSelection"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__RunSelectorWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   40,    2, 0x08 /* Private */,
       5,    0,   43,    2, 0x08 /* Private */,
       6,    0,   44,    2, 0x08 /* Private */,
       7,    0,   45,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Picto::RunSelectorWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RunSelectorWidget *_t = static_cast<RunSelectorWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->runSelectionChanged(); break;
        case 1: _t->buttonClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->selectAll(); break;
        case 3: _t->selectSaved(); break;
        case 4: _t->clearSelection(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (RunSelectorWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&RunSelectorWidget::runSelectionChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject Picto::RunSelectorWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Picto__RunSelectorWidget.data,
      qt_meta_data_Picto__RunSelectorWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::RunSelectorWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::RunSelectorWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__RunSelectorWidget.stringdata0))
        return static_cast<void*>(const_cast< RunSelectorWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int Picto::RunSelectorWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void Picto::RunSelectorWidget::runSelectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
