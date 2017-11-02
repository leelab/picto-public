/****************************************************************************
** Meta object code from reading C++ file 'EditorState.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../source/common/designer/EditorState.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QSharedPointer>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EditorState.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_EditorState_t {
    QByteArrayData data[55];
    char stringdata0[806];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EditorState_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EditorState_t qt_meta_stringdata_EditorState = {
    {
QT_MOC_LITERAL(0, 0, 11), // "EditorState"
QT_MOC_LITERAL(1, 12, 15), // "editModeChanged"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 4), // "mode"
QT_MOC_LITERAL(4, 34, 11), // "zoomChanged"
QT_MOC_LITERAL(5, 46, 4), // "zoom"
QT_MOC_LITERAL(6, 51, 16), // "lineColorChanged"
QT_MOC_LITERAL(7, 68, 5), // "color"
QT_MOC_LITERAL(8, 74, 24), // "backgroundPatternChanged"
QT_MOC_LITERAL(9, 99, 7), // "pattern"
QT_MOC_LITERAL(10, 107, 20), // "insertionItemChanged"
QT_MOC_LITERAL(11, 128, 9), // "className"
QT_MOC_LITERAL(12, 138, 12), // "friendlyName"
QT_MOC_LITERAL(13, 151, 17), // "windowItemsLoaded"
QT_MOC_LITERAL(14, 169, 19), // "releaseEditorMemory"
QT_MOC_LITERAL(15, 189, 18), // "windowAssetChanged"
QT_MOC_LITERAL(16, 208, 21), // "QSharedPointer<Asset>"
QT_MOC_LITERAL(17, 230, 5), // "asset"
QT_MOC_LITERAL(18, 236, 20), // "selectedAssetChanged"
QT_MOC_LITERAL(19, 257, 12), // "itemSelected"
QT_MOC_LITERAL(20, 270, 14), // "QGraphicsItem*"
QT_MOC_LITERAL(21, 285, 4), // "item"
QT_MOC_LITERAL(22, 290, 22), // "currentAnalysisChanged"
QT_MOC_LITERAL(23, 313, 24), // "QSharedPointer<Analysis>"
QT_MOC_LITERAL(24, 338, 12), // "currAnalysis"
QT_MOC_LITERAL(25, 351, 17), // "arrowPortSelected"
QT_MOC_LITERAL(26, 369, 12), // "itemInserted"
QT_MOC_LITERAL(27, 382, 23), // "undoableActionPerformed"
QT_MOC_LITERAL(28, 406, 13), // "undoRequested"
QT_MOC_LITERAL(29, 420, 15), // "resetExperiment"
QT_MOC_LITERAL(30, 436, 15), // "searchRequested"
QT_MOC_LITERAL(31, 452, 13), // "SearchRequest"
QT_MOC_LITERAL(32, 466, 13), // "searchRequest"
QT_MOC_LITERAL(33, 480, 16), // "setTopLevelAsset"
QT_MOC_LITERAL(34, 497, 13), // "topLevelAsset"
QT_MOC_LITERAL(35, 511, 18), // "setCurrentAnalysis"
QT_MOC_LITERAL(36, 530, 11), // "setEditMode"
QT_MOC_LITERAL(37, 542, 7), // "setZoom"
QT_MOC_LITERAL(38, 550, 12), // "setLineColor"
QT_MOC_LITERAL(39, 563, 16), // "setInsertionItem"
QT_MOC_LITERAL(40, 580, 8), // "category"
QT_MOC_LITERAL(41, 589, 4), // "type"
QT_MOC_LITERAL(42, 594, 6), // "pixmap"
QT_MOC_LITERAL(43, 601, 14), // "setWindowAsset"
QT_MOC_LITERAL(44, 616, 8), // "undoable"
QT_MOC_LITERAL(45, 625, 22), // "setWindowAssetToParent"
QT_MOC_LITERAL(46, 648, 20), // "setWindowItemsLoaded"
QT_MOC_LITERAL(47, 669, 16), // "setSelectedAsset"
QT_MOC_LITERAL(48, 686, 15), // "setSelectedItem"
QT_MOC_LITERAL(49, 702, 21), // "setLastActionUndoable"
QT_MOC_LITERAL(50, 724, 12), // "reloadWindow"
QT_MOC_LITERAL(51, 737, 22), // "triggerExperimentReset"
QT_MOC_LITERAL(52, 760, 19), // "triggerItemInserted"
QT_MOC_LITERAL(53, 780, 11), // "requestUndo"
QT_MOC_LITERAL(54, 792, 13) // "requestSearch"

    },
    "EditorState\0editModeChanged\0\0mode\0"
    "zoomChanged\0zoom\0lineColorChanged\0"
    "color\0backgroundPatternChanged\0pattern\0"
    "insertionItemChanged\0className\0"
    "friendlyName\0windowItemsLoaded\0"
    "releaseEditorMemory\0windowAssetChanged\0"
    "QSharedPointer<Asset>\0asset\0"
    "selectedAssetChanged\0itemSelected\0"
    "QGraphicsItem*\0item\0currentAnalysisChanged\0"
    "QSharedPointer<Analysis>\0currAnalysis\0"
    "arrowPortSelected\0itemInserted\0"
    "undoableActionPerformed\0undoRequested\0"
    "resetExperiment\0searchRequested\0"
    "SearchRequest\0searchRequest\0"
    "setTopLevelAsset\0topLevelAsset\0"
    "setCurrentAnalysis\0setEditMode\0setZoom\0"
    "setLineColor\0setInsertionItem\0category\0"
    "type\0pixmap\0setWindowAsset\0undoable\0"
    "setWindowAssetToParent\0setWindowItemsLoaded\0"
    "setSelectedAsset\0setSelectedItem\0"
    "setLastActionUndoable\0reloadWindow\0"
    "triggerExperimentReset\0triggerItemInserted\0"
    "requestUndo\0requestSearch"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EditorState[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      40,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      17,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  214,    2, 0x06 /* Public */,
       4,    1,  217,    2, 0x06 /* Public */,
       6,    1,  220,    2, 0x06 /* Public */,
       8,    1,  223,    2, 0x06 /* Public */,
      10,    2,  226,    2, 0x06 /* Public */,
      13,    0,  231,    2, 0x06 /* Public */,
      14,    0,  232,    2, 0x06 /* Public */,
      15,    1,  233,    2, 0x06 /* Public */,
      18,    1,  236,    2, 0x06 /* Public */,
      19,    1,  239,    2, 0x06 /* Public */,
      22,    1,  242,    2, 0x06 /* Public */,
      25,    1,  245,    2, 0x06 /* Public */,
      26,    0,  248,    2, 0x06 /* Public */,
      27,    0,  249,    2, 0x06 /* Public */,
      28,    0,  250,    2, 0x06 /* Public */,
      29,    0,  251,    2, 0x06 /* Public */,
      30,    1,  252,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      33,    1,  255,    2, 0x0a /* Public */,
      35,    1,  258,    2, 0x0a /* Public */,
      36,    1,  261,    2, 0x0a /* Public */,
      37,    1,  264,    2, 0x0a /* Public */,
      38,    1,  267,    2, 0x0a /* Public */,
      39,    5,  270,    2, 0x0a /* Public */,
      39,    4,  281,    2, 0x2a /* Public | MethodCloned */,
      39,    3,  290,    2, 0x2a /* Public | MethodCloned */,
      39,    2,  297,    2, 0x2a /* Public | MethodCloned */,
      39,    1,  302,    2, 0x2a /* Public | MethodCloned */,
      39,    0,  305,    2, 0x2a /* Public | MethodCloned */,
      43,    2,  306,    2, 0x0a /* Public */,
      43,    1,  311,    2, 0x2a /* Public | MethodCloned */,
      45,    0,  314,    2, 0x0a /* Public */,
      46,    0,  315,    2, 0x0a /* Public */,
      47,    1,  316,    2, 0x0a /* Public */,
      48,    1,  319,    2, 0x0a /* Public */,
      49,    0,  322,    2, 0x0a /* Public */,
      50,    0,  323,    2, 0x0a /* Public */,
      51,    0,  324,    2, 0x0a /* Public */,
      52,    0,  325,    2, 0x0a /* Public */,
      53,    0,  326,    2, 0x0a /* Public */,
      54,    1,  327,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::QColor,    7,
    QMetaType::Void, QMetaType::QPixmap,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   11,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 20,   21,
    QMetaType::Void, 0x80000000 | 23,   24,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 31,   32,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 16,   34,
    QMetaType::Bool, 0x80000000 | 23,   24,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Double, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::QColor,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QPixmap,   11,   12,   40,   41,   42,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString,   11,   12,   40,   41,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,   11,   12,   40,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   11,   12,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16, QMetaType::Bool,   17,   44,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 20,   21,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 31,   32,

       0        // eod
};

void EditorState::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EditorState *_t = static_cast<EditorState *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->editModeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->zoomChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->lineColorChanged((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 3: _t->backgroundPatternChanged((*reinterpret_cast< QPixmap(*)>(_a[1]))); break;
        case 4: _t->insertionItemChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 5: _t->windowItemsLoaded(); break;
        case 6: _t->releaseEditorMemory(); break;
        case 7: _t->windowAssetChanged((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
        case 8: _t->selectedAssetChanged((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
        case 9: _t->itemSelected((*reinterpret_cast< QGraphicsItem*(*)>(_a[1]))); break;
        case 10: _t->currentAnalysisChanged((*reinterpret_cast< QSharedPointer<Analysis>(*)>(_a[1]))); break;
        case 11: _t->arrowPortSelected((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
        case 12: _t->itemInserted(); break;
        case 13: _t->undoableActionPerformed(); break;
        case 14: _t->undoRequested(); break;
        case 15: _t->resetExperiment(); break;
        case 16: _t->searchRequested((*reinterpret_cast< SearchRequest(*)>(_a[1]))); break;
        case 17: _t->setTopLevelAsset((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
        case 18: { bool _r = _t->setCurrentAnalysis((*reinterpret_cast< QSharedPointer<Analysis>(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 19: _t->setEditMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: { double _r = _t->setZoom((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 21: _t->setLineColor((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 22: _t->setInsertionItem((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])),(*reinterpret_cast< QPixmap(*)>(_a[5]))); break;
        case 23: _t->setInsertionItem((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4]))); break;
        case 24: _t->setInsertionItem((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 25: _t->setInsertionItem((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 26: _t->setInsertionItem((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 27: _t->setInsertionItem(); break;
        case 28: _t->setWindowAsset((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 29: _t->setWindowAsset((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
        case 30: _t->setWindowAssetToParent(); break;
        case 31: _t->setWindowItemsLoaded(); break;
        case 32: _t->setSelectedAsset((*reinterpret_cast< QSharedPointer<Asset>(*)>(_a[1]))); break;
        case 33: _t->setSelectedItem((*reinterpret_cast< QGraphicsItem*(*)>(_a[1]))); break;
        case 34: _t->setLastActionUndoable(); break;
        case 35: _t->reloadWindow(); break;
        case 36: _t->triggerExperimentReset(); break;
        case 37: _t->triggerItemInserted(); break;
        case 38: _t->requestUndo(); break;
        case 39: _t->requestSearch((*reinterpret_cast< SearchRequest(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Asset> >(); break;
            }
            break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Asset> >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QGraphicsItem* >(); break;
            }
            break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Analysis> >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Asset> >(); break;
            }
            break;
        case 17:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Asset> >(); break;
            }
            break;
        case 18:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Analysis> >(); break;
            }
            break;
        case 28:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Asset> >(); break;
            }
            break;
        case 29:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Asset> >(); break;
            }
            break;
        case 32:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSharedPointer<Asset> >(); break;
            }
            break;
        case 33:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QGraphicsItem* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (EditorState::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorState::editModeChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (EditorState::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorState::zoomChanged)) {
                *result = 1;
            }
        }
        {
            typedef void (EditorState::*_t)(QColor );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorState::lineColorChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (EditorState::*_t)(QPixmap );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorState::backgroundPatternChanged)) {
                *result = 3;
            }
        }
        {
            typedef void (EditorState::*_t)(QString , QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorState::insertionItemChanged)) {
                *result = 4;
            }
        }
        {
            typedef void (EditorState::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorState::windowItemsLoaded)) {
                *result = 5;
            }
        }
        {
            typedef void (EditorState::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorState::releaseEditorMemory)) {
                *result = 6;
            }
        }
        {
            typedef void (EditorState::*_t)(QSharedPointer<Asset> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorState::windowAssetChanged)) {
                *result = 7;
            }
        }
        {
            typedef void (EditorState::*_t)(QSharedPointer<Asset> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorState::selectedAssetChanged)) {
                *result = 8;
            }
        }
        {
            typedef void (EditorState::*_t)(QGraphicsItem * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorState::itemSelected)) {
                *result = 9;
            }
        }
        {
            typedef void (EditorState::*_t)(QSharedPointer<Analysis> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorState::currentAnalysisChanged)) {
                *result = 10;
            }
        }
        {
            typedef void (EditorState::*_t)(QSharedPointer<Asset> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorState::arrowPortSelected)) {
                *result = 11;
            }
        }
        {
            typedef void (EditorState::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorState::itemInserted)) {
                *result = 12;
            }
        }
        {
            typedef void (EditorState::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorState::undoableActionPerformed)) {
                *result = 13;
            }
        }
        {
            typedef void (EditorState::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorState::undoRequested)) {
                *result = 14;
            }
        }
        {
            typedef void (EditorState::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorState::resetExperiment)) {
                *result = 15;
            }
        }
        {
            typedef void (EditorState::*_t)(SearchRequest );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditorState::searchRequested)) {
                *result = 16;
            }
        }
    }
}

const QMetaObject EditorState::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_EditorState.data,
      qt_meta_data_EditorState,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *EditorState::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EditorState::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_EditorState.stringdata0))
        return static_cast<void*>(const_cast< EditorState*>(this));
    return QObject::qt_metacast(_clname);
}

int EditorState::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 40)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 40;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 40)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 40;
    }
    return _id;
}

// SIGNAL 0
void EditorState::editModeChanged(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void EditorState::zoomChanged(double _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void EditorState::lineColorChanged(QColor _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void EditorState::backgroundPatternChanged(QPixmap _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void EditorState::insertionItemChanged(QString _t1, QString _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void EditorState::windowItemsLoaded()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void EditorState::releaseEditorMemory()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}

// SIGNAL 7
void EditorState::windowAssetChanged(QSharedPointer<Asset> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void EditorState::selectedAssetChanged(QSharedPointer<Asset> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void EditorState::itemSelected(QGraphicsItem * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void EditorState::currentAnalysisChanged(QSharedPointer<Analysis> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void EditorState::arrowPortSelected(QSharedPointer<Asset> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void EditorState::itemInserted()
{
    QMetaObject::activate(this, &staticMetaObject, 12, Q_NULLPTR);
}

// SIGNAL 13
void EditorState::undoableActionPerformed()
{
    QMetaObject::activate(this, &staticMetaObject, 13, Q_NULLPTR);
}

// SIGNAL 14
void EditorState::undoRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 14, Q_NULLPTR);
}

// SIGNAL 15
void EditorState::resetExperiment()
{
    QMetaObject::activate(this, &staticMetaObject, 15, Q_NULLPTR);
}

// SIGNAL 16
void EditorState::searchRequested(SearchRequest _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}
QT_END_MOC_NAMESPACE
