/****************************************************************************
** Meta object code from reading C++ file 'waterivengine.h'
**
** Created: Tue Mar 20 23:43:44 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "waterivengine.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'waterivengine.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WaterIVEngine[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      21,   15,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_WaterIVEngine[] = {
    "WaterIVEngine\0\0reply\0"
    "dataFetchComplete(QNetworkReply*)\0"
};

const QMetaObject WaterIVEngine::staticMetaObject = {
    { &Plasma::DataEngine::staticMetaObject, qt_meta_stringdata_WaterIVEngine,
      qt_meta_data_WaterIVEngine, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &WaterIVEngine::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *WaterIVEngine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *WaterIVEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WaterIVEngine))
        return static_cast<void*>(const_cast< WaterIVEngine*>(this));
    typedef Plasma::DataEngine QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int WaterIVEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Plasma::DataEngine QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dataFetchComplete((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
