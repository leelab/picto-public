/****************************************************************************
** Meta object code from reading C++ file 'ImageGraphic.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/stimuli/ImageGraphic.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ImageGraphic.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__ImageGraphic_t {
    QByteArrayData data[8];
    char stringdata0[57];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__ImageGraphic_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__ImageGraphic_t qt_meta_stringdata_Picto__ImageGraphic = {
    {
QT_MOC_LITERAL(0, 0, 19), // "Picto::ImageGraphic"
QT_MOC_LITERAL(1, 20, 13), // "setDimensions"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 1), // "w"
QT_MOC_LITERAL(4, 37, 1), // "h"
QT_MOC_LITERAL(5, 39, 5), // "width"
QT_MOC_LITERAL(6, 45, 6), // "height"
QT_MOC_LITERAL(7, 52, 4) // "file"

    },
    "Picto::ImageGraphic\0setDimensions\0\0w\0"
    "h\0width\0height\0file"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__ImageGraphic[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       3,   24, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,

 // properties: name, type, flags
       5, QMetaType::Int, 0x00095103,
       6, QMetaType::Int, 0x00095103,
       7, QMetaType::QString, 0x00095103,

       0        // eod
};

void Picto::ImageGraphic::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ImageGraphic *_t = static_cast<ImageGraphic *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setDimensions((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        ImageGraphic *_t = static_cast<ImageGraphic *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getWidth(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->getHeight(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->getFile(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        ImageGraphic *_t = static_cast<ImageGraphic *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setWidth(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setHeight(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setFile(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject Picto::ImageGraphic::staticMetaObject = {
    { &VisualElement::staticMetaObject, qt_meta_stringdata_Picto__ImageGraphic.data,
      qt_meta_data_Picto__ImageGraphic,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::ImageGraphic::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::ImageGraphic::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__ImageGraphic.stringdata0))
        return static_cast<void*>(const_cast< ImageGraphic*>(this));
    return VisualElement::qt_metacast(_clname);
}

int Picto::ImageGraphic::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = VisualElement::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
