/****************************************************************************
** Meta object code from reading C++ file 'CircleGraphic.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/stimuli/CircleGraphic.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CircleGraphic.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Picto__CircleGraphic_t {
    QByteArrayData data[3];
    char stringdata0[36];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Picto__CircleGraphic_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Picto__CircleGraphic_t qt_meta_stringdata_Picto__CircleGraphic = {
    {
QT_MOC_LITERAL(0, 0, 20), // "Picto::CircleGraphic"
QT_MOC_LITERAL(1, 21, 6), // "radius"
QT_MOC_LITERAL(2, 28, 7) // "outline"

    },
    "Picto::CircleGraphic\0radius\0outline"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Picto__CircleGraphic[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       2,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::Int, 0x00095103,
       2, QMetaType::Bool, 0x00095103,

       0        // eod
};

void Picto::CircleGraphic::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        CircleGraphic *_t = static_cast<CircleGraphic *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getRadius(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->getOutline(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        CircleGraphic *_t = static_cast<CircleGraphic *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setRadius(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setOutline(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Picto::CircleGraphic::staticMetaObject = {
    { &VisualElement::staticMetaObject, qt_meta_stringdata_Picto__CircleGraphic.data,
      qt_meta_data_Picto__CircleGraphic,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Picto::CircleGraphic::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Picto::CircleGraphic::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Picto__CircleGraphic.stringdata0))
        return static_cast<void*>(const_cast< CircleGraphic*>(this));
    return VisualElement::qt_metacast(_clname);
}

int Picto::CircleGraphic::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = VisualElement::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
   if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
