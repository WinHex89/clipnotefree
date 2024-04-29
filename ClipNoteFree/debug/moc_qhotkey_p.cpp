/****************************************************************************
** Meta object code from reading C++ file 'qhotkey_p.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../QHotkey/qhotkey_p.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qhotkey_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSQHotkeyPrivateENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSQHotkeyPrivateENDCLASS = QtMocHelpers::stringData(
    "QHotkeyPrivate",
    "addMappingInvoked",
    "",
    "Qt::Key",
    "keycode",
    "Qt::KeyboardModifiers",
    "modifiers",
    "QHotkey::NativeShortcut",
    "nativeShortcut",
    "addShortcutInvoked",
    "QHotkey*",
    "hotkey",
    "removeShortcutInvoked",
    "nativeShortcutInvoked"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSQHotkeyPrivateENDCLASS_t {
    uint offsetsAndSizes[28];
    char stringdata0[15];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[8];
    char stringdata4[8];
    char stringdata5[22];
    char stringdata6[10];
    char stringdata7[24];
    char stringdata8[15];
    char stringdata9[19];
    char stringdata10[9];
    char stringdata11[7];
    char stringdata12[22];
    char stringdata13[22];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSQHotkeyPrivateENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSQHotkeyPrivateENDCLASS_t qt_meta_stringdata_CLASSQHotkeyPrivateENDCLASS = {
    {
        QT_MOC_LITERAL(0, 14),  // "QHotkeyPrivate"
        QT_MOC_LITERAL(15, 17),  // "addMappingInvoked"
        QT_MOC_LITERAL(33, 0),  // ""
        QT_MOC_LITERAL(34, 7),  // "Qt::Key"
        QT_MOC_LITERAL(42, 7),  // "keycode"
        QT_MOC_LITERAL(50, 21),  // "Qt::KeyboardModifiers"
        QT_MOC_LITERAL(72, 9),  // "modifiers"
        QT_MOC_LITERAL(82, 23),  // "QHotkey::NativeShortcut"
        QT_MOC_LITERAL(106, 14),  // "nativeShortcut"
        QT_MOC_LITERAL(121, 18),  // "addShortcutInvoked"
        QT_MOC_LITERAL(140, 8),  // "QHotkey*"
        QT_MOC_LITERAL(149, 6),  // "hotkey"
        QT_MOC_LITERAL(156, 21),  // "removeShortcutInvoked"
        QT_MOC_LITERAL(178, 21)   // "nativeShortcutInvoked"
    },
    "QHotkeyPrivate",
    "addMappingInvoked",
    "",
    "Qt::Key",
    "keycode",
    "Qt::KeyboardModifiers",
    "modifiers",
    "QHotkey::NativeShortcut",
    "nativeShortcut",
    "addShortcutInvoked",
    "QHotkey*",
    "hotkey",
    "removeShortcutInvoked",
    "nativeShortcutInvoked"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSQHotkeyPrivateENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
       1,    3,   38,    2, 0x00,    1 /* Private */,
       9,    1,   45,    2, 0x00,    5 /* Private */,
      12,    1,   48,    2, 0x00,    7 /* Private */,
      13,    2,   51,    2, 0x00,    9 /* Private */,

 // methods: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5, 0x80000000 | 7,    4,    6,    8,
    QMetaType::Bool, 0x80000000 | 10,   11,
    QMetaType::Bool, 0x80000000 | 10,   11,
    0x80000000 | 7, 0x80000000 | 3, 0x80000000 | 5,    4,    6,

       0        // eod
};

Q_CONSTINIT const QMetaObject QHotkeyPrivate::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSQHotkeyPrivateENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSQHotkeyPrivateENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSQHotkeyPrivateENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<QHotkeyPrivate, std::true_type>,
        // method 'addMappingInvoked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Qt::Key, std::false_type>,
        QtPrivate::TypeAndForceComplete<Qt::KeyboardModifiers, std::false_type>,
        QtPrivate::TypeAndForceComplete<QHotkey::NativeShortcut, std::false_type>,
        // method 'addShortcutInvoked'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<QHotkey *, std::false_type>,
        // method 'removeShortcutInvoked'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<QHotkey *, std::false_type>,
        // method 'nativeShortcutInvoked'
        QtPrivate::TypeAndForceComplete<QHotkey::NativeShortcut, std::false_type>,
        QtPrivate::TypeAndForceComplete<Qt::Key, std::false_type>,
        QtPrivate::TypeAndForceComplete<Qt::KeyboardModifiers, std::false_type>
    >,
    nullptr
} };

void QHotkeyPrivate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QHotkeyPrivate *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->addMappingInvoked((*reinterpret_cast< std::add_pointer_t<Qt::Key>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<Qt::KeyboardModifiers>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QHotkey::NativeShortcut>>(_a[3]))); break;
        case 1: { bool _r = _t->addShortcutInvoked((*reinterpret_cast< std::add_pointer_t<QHotkey*>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 2: { bool _r = _t->removeShortcutInvoked((*reinterpret_cast< std::add_pointer_t<QHotkey*>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 3: { QHotkey::NativeShortcut _r = _t->nativeShortcutInvoked((*reinterpret_cast< std::add_pointer_t<Qt::Key>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<Qt::KeyboardModifiers>>(_a[2])));
            if (_a[0]) *reinterpret_cast< QHotkey::NativeShortcut*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 2:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QHotkey::NativeShortcut >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QHotkey* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QHotkey* >(); break;
            }
            break;
        }
    }
}

const QMetaObject *QHotkeyPrivate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QHotkeyPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSQHotkeyPrivateENDCLASS.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QAbstractNativeEventFilter"))
        return static_cast< QAbstractNativeEventFilter*>(this);
    return QObject::qt_metacast(_clname);
}

int QHotkeyPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
