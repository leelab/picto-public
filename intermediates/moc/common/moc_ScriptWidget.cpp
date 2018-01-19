/****************************************************************************
** Meta object code from reading C++ file 'ScriptWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/ScriptWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ScriptWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScriptWidget_t {
    QByteArrayData data[10];
    char stringdata0[125];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScriptWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScriptWidget_t qt_meta_stringdata_ScriptWidget = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ScriptWidget"
QT_MOC_LITERAL(1, 13, 10), // "textEdited"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 4), // "text"
QT_MOC_LITERAL(4, 30, 15), // "editingFinished"
QT_MOC_LITERAL(5, 46, 15), // "scriptLostFocus"
QT_MOC_LITERAL(6, 62, 18), // "textChangeDetected"
QT_MOC_LITERAL(7, 81, 15), // "searchRequested"
QT_MOC_LITERAL(8, 97, 13), // "SearchRequest"
QT_MOC_LITERAL(9, 111, 13) // "searchRequest"

    },
    "ScriptWidget\0textEdited\0\0text\0"
    "editingFinished\0scriptLostFocus\0"
    "textChangeDetected\0searchRequested\0"
    "SearchRequest\0searchRequest"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScriptWidget[] = {

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
       4,    0,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   43,    2, 0x0a /* Public */,
       6,    0,   44,    2, 0x08 /* Private */,
       7,    1,   45,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,

       0        // eod
};

void ScriptWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ScriptWidget *_t = static_cast<ScriptWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->editingFinished(); break;
        case 2: _t->scriptLostFocus(); break;
        case 3: _t->textChangeDetected(); break;
        case 4: _t->searchRequested((*reinterpret_cast< SearchRequest(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ScriptWidget::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ScriptWidget::textEdited)) {
                *result = 0;
            }
        }
        {
            typedef void (ScriptWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ScriptWidget::editingFinished)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject ScriptWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ScriptWidget.data,
      qt_meta_data_ScriptWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScriptWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScriptWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScriptWidget.stringdata0))
        return static_cast<void*>(const_cast< ScriptWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int ScriptWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void ScriptWidget::textEdited(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ScriptWidget::editingFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
