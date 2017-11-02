/****************************************************************************
** Meta object code from reading C++ file 'AnalysisSelectorWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/AnalysisSelectorWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AnalysisSelectorWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__AnalysisSelectorWidget_t {
    QByteArrayData data[10];
    char stringdata0[144];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__AnalysisSelectorWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__AnalysisSelectorWidget_t qt_meta_stringdata_Picto__AnalysisSelectorWidget = {
    {
QT_MOC_LITERAL(0, 0, 29), // "Picto::AnalysisSelectorWidget"
QT_MOC_LITERAL(1, 30, 21), // "analysisWidgetChanged"
QT_MOC_LITERAL(2, 52, 0), // ""
QT_MOC_LITERAL(3, 53, 23), // "notifyAnalysisSelection"
QT_MOC_LITERAL(4, 77, 4), // "name"
QT_MOC_LITERAL(5, 82, 8), // "selected"
QT_MOC_LITERAL(6, 91, 5), // "local"
QT_MOC_LITERAL(7, 97, 22), // "clearAnalysisSelection"
QT_MOC_LITERAL(8, 120, 15), // "checkboxChanged"
QT_MOC_LITERAL(9, 136, 7) // "checked"

    },
    "Picto::AnalysisSelectorWidget\0"
    "analysisWidgetChanged\0\0notifyAnalysisSelection\0"
    "name\0selected\0local\0clearAnalysisSelection\0"
    "checkboxChanged\0checked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__AnalysisSelectorWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    3,   35,    2, 0x06 /* Public */,
       7,    1,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   45,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool, QMetaType::Bool,    4,    5,    6,
    QMetaType::Void, QMetaType::Bool,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    9,

       0        // eod
};

void Picto::AnalysisSelectorWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AnalysisSelectorWidget *_t = static_cast<AnalysisSelectorWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->analysisWidgetChanged(); break;
        case 1: _t->notifyAnalysisSelection((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 2: _t->clearAnalysisSelection((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->checkboxChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AnalysisSelectorWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AnalysisSelectorWidget::analysisWidgetChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (AnalysisSelectorWidget::*_t)(const QString & , bool , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AnalysisSelectorWidget::notifyAnalysisSelection)) {
                *result = 1;
            }
        }
        {
            typedef void (AnalysisSelectorWidget::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AnalysisSelectorWidget::clearAnalysisSelection)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject Picto::AnalysisSelectorWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Picto__AnalysisSelectorWidget.data,
      qt_meta_data_Picto__AnalysisSelectorWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::AnalysisSelectorWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::AnalysisSelectorWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__AnalysisSelectorWidget.stringdata0))
        return static_cast<void*>(const_cast< AnalysisSelectorWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int Picto::AnalysisSelectorWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Picto::AnalysisSelectorWidget::analysisWidgetChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void Picto::AnalysisSelectorWidget::notifyAnalysisSelection(const QString & _t1, bool _t2, bool _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Picto::AnalysisSelectorWidget::clearAnalysisSelection(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
