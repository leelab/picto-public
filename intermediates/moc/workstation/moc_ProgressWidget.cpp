/****************************************************************************
** Meta object code from reading C++ file 'ProgressWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/workstation/ReplayViewer/ProgressWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ProgressWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ProgressWidget_t {
    QByteArrayData data[10];
    char stringdata0[115];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ProgressWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ProgressWidget_t qt_meta_stringdata_ProgressWidget = {
    {
QT_MOC_LITERAL(0, 0, 14), // "ProgressWidget"
QT_MOC_LITERAL(1, 15, 10), // "userAction"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 9), // "mouseDown"
QT_MOC_LITERAL(4, 37, 14), // "valueRequested"
QT_MOC_LITERAL(5, 52, 8), // "progress"
QT_MOC_LITERAL(6, 61, 13), // "sliderPressed"
QT_MOC_LITERAL(7, 75, 14), // "sliderReleased"
QT_MOC_LITERAL(8, 90, 18), // "sliderValueChanged"
QT_MOC_LITERAL(9, 109, 5) // "value"

    },
    "ProgressWidget\0userAction\0\0mouseDown\0"
    "valueRequested\0progress\0sliderPressed\0"
    "sliderReleased\0sliderValueChanged\0"
    "value"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProgressWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    1,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   45,    2, 0x08 /* Private */,
       7,    0,   46,    2, 0x08 /* Private */,
       8,    1,   47,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Double,    5,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    9,

       0        // eod
};

void ProgressWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ProgressWidget *_t = static_cast<ProgressWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->userAction((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->valueRequested((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->sliderPressed(); break;
        case 3: _t->sliderReleased(); break;
        case 4: _t->sliderValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ProgressWidget::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ProgressWidget::userAction)) {
                *result = 0;
            }
        }
        {
            typedef void (ProgressWidget::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ProgressWidget::valueRequested)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject ProgressWidget::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_ProgressWidget.data,
      qt_meta_data_ProgressWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ProgressWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProgressWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ProgressWidget.stringdata0))
        return static_cast<void*>(const_cast< ProgressWidget*>(this));
    return QFrame::qt_metacast(_clname);
}

int ProgressWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
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
void ProgressWidget::userAction(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ProgressWidget::valueRequested(double _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
