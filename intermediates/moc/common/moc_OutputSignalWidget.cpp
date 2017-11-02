/****************************************************************************
** Meta object code from reading C++ file 'OutputSignalWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/compositor/OutputSignalWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OutputSignalWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__OutputSignalWidget_t {
    QByteArrayData data[6];
    char stringdata0[66];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__OutputSignalWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__OutputSignalWidget_t qt_meta_stringdata_Picto__OutputSignalWidget = {
    {
QT_MOC_LITERAL(0, 0, 25), // "Picto::OutputSignalWidget"
QT_MOC_LITERAL(1, 26, 19), // "outputSignalChanged"
QT_MOC_LITERAL(2, 46, 0), // ""
QT_MOC_LITERAL(3, 47, 5), // "pinId"
QT_MOC_LITERAL(4, 53, 7), // "enabled"
QT_MOC_LITERAL(5, 61, 4) // "high"

    },
    "Picto::OutputSignalWidget\0outputSignalChanged\0"
    "\0pinId\0enabled\0high"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__OutputSignalWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    3,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Bool, QMetaType::Bool,    3,    4,    5,

       0        // eod
};

void Picto::OutputSignalWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OutputSignalWidget *_t = static_cast<OutputSignalWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->outputSignalChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject Picto::OutputSignalWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Picto__OutputSignalWidget.data,
      qt_meta_data_Picto__OutputSignalWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::OutputSignalWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::OutputSignalWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__OutputSignalWidget.stringdata0))
        return static_cast<void*>(const_cast< OutputSignalWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int Picto::OutputSignalWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
