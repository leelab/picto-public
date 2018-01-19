/****************************************************************************
** Meta object code from reading C++ file 'diagramscene.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/diagramscene.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'diagramscene.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DiagramScene_t {
    QByteArrayData data[21];
    char stringdata0[296];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DiagramScene_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DiagramScene_t qt_meta_stringdata_DiagramScene = {
    {
QT_MOC_LITERAL(0, 0, 12), // "DiagramScene"
QT_MOC_LITERAL(1, 13, 12), // "itemInserted"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 12), // "DiagramItem*"
QT_MOC_LITERAL(4, 40, 4), // "item"
QT_MOC_LITERAL(5, 45, 12), // "itemSelected"
QT_MOC_LITERAL(6, 58, 14), // "QGraphicsItem*"
QT_MOC_LITERAL(7, 73, 13), // "assetSelected"
QT_MOC_LITERAL(8, 87, 21), // "QSharedPointer<Asset>"
QT_MOC_LITERAL(9, 109, 5), // "asset"
QT_MOC_LITERAL(10, 115, 13), // "setSceneAsset"
QT_MOC_LITERAL(11, 129, 12), // "setLineColor"
QT_MOC_LITERAL(12, 142, 5), // "color"
QT_MOC_LITERAL(13, 148, 15), // "editorLostFocus"
QT_MOC_LITERAL(14, 164, 16), // "DiagramTextItem*"
QT_MOC_LITERAL(15, 181, 19), // "deleteSelectedItems"
QT_MOC_LITERAL(16, 201, 17), // "copySelectedItems"
QT_MOC_LITERAL(17, 219, 26), // "analysisImportSelectedItem"
QT_MOC_LITERAL(18, 246, 26), // "analysisExportSelectedItem"
QT_MOC_LITERAL(19, 273, 10), // "pasteItems"
QT_MOC_LITERAL(20, 284, 11) // "clearAssets"

    },
    "DiagramScene\0itemInserted\0\0DiagramItem*\0"
    "item\0itemSelected\0QGraphicsItem*\0"
    "assetSelected\0QSharedPointer<Asset>\0"
    "asset\0setSceneAsset\0setLineColor\0color\0"
    "editorLostFocus\0DiagramTextItem*\0"
    "deleteSelectedItems\0copySelectedItems\0"
    "analysisImportSelectedItem\0"
    "analysisExportSelectedItem\0pasteItems\0"
    "clearAssets"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DiagramScene[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x06 /* Public */,
       5,    1,   77,    2, 0x06 /* Public */,
       7,    1,   80,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,   83,    2, 0x0a /* Public */,
      11,    1,   86,    2, 0x0a /* Public */,
      13,    1,   89,    2, 0x0a /* Public */,
      15,    0,   92,    2, 0x0a /* Public */,
      16,    0,   93,    2, 0x0a /* Public */,
      17,    0,   94,    2, 0x0a /* Public */,
      18,    0,   95,    2, 0x0a /* Public */,
      19,    0,   96,    2, 0x0a /* Public */,
      20,    0,   97,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    4,
    QMetaType::Void, 0x80000000 | 8,    9,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::QColor,   12,
    QMetaType::Void, 0x80000000 | 14,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DiagramScene::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DiagramScene *_t = static_cast<DiagramScene *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->itemInserted((*reinterpret_cast< DiagramItem*(*)>(_a[1]))); break;
        case 1: _t->itemSelected((*reinterpret_cast< QGraphicsItem*(*)>(_a[1]))); break;
        case 2: _t->assetSelected((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
        case 3: _t->setSceneAsset((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
        case 4: _t->setLineColor((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 5: _t->editorLostFocus((*reinterpret_cast< DiagramTextItem*(*)>(_a[1]))); break;
        case 6: _t->deleteSelectedItems(); break;
        case 7: _t->copySelectedItems(); break;
        case 8: _t->analysisImportSelectedItem(); break;
        case 9: _t->analysisExportSelectedItem(); break;
        case 10: _t->pasteItems(); break;
        case 11: _t->clearAssets(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< DiagramItem* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QGraphicsItem* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Asset> >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Asset> >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< DiagramTextItem* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DiagramScene::*_t)(DiagramItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DiagramScene::itemInserted)) {
                *result = 0;
            }
        }
        {
            typedef void (DiagramScene::*_t)(QGraphicsItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DiagramScene::itemSelected)) {
                *result = 1;
            }
        }
        {
            typedef void (DiagramScene::*_t)(QSharedPointer<Asset> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DiagramScene::assetSelected)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject DiagramScene::staticMetaObject = {
    { &QGraphicsScene::staticMetaObject, qt_meta_stringdata_DiagramScene.data,
      qt_meta_data_DiagramScene,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DiagramScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DiagramScene::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DiagramScene.stringdata0))
        return static_cast<void*>(const_cast< DiagramScene*>(this));
    return QGraphicsScene::qt_metacast(_clname);
}

int DiagramScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void DiagramScene::itemInserted(DiagramItem * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DiagramScene::itemSelected(QGraphicsItem * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DiagramScene::assetSelected(QSharedPointer<Asset> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
