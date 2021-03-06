#include <QString>
#include <QDataStream>
#include "ilwis.h"
#include "identity.h"

using namespace Ilwis;

qint64 Identity::_baseId = 0;


Identity::Identity() : _id(i64UNDEF), _name(sUNDEF), _code(sUNDEF){
}

Identity::Identity(const QString &name, qint64 id, const QString &cde, const QString &descr)
{
    _name = name;
    _id = id;
    _code = cde;
    _description = descr;
}

quint64 Identity::id() const
{
    return _id;
}

QString Identity::name() const
{
    return _name;
}

void Identity::setName(const QString &n)
{
    _name =  n;
}

QString Identity::description() const
{
    return _description;
}

void Identity::setDescription(const QString &desc)
{
    _description = desc;
}

void Identity::setCode(const QString &code)
{
    _code = code;
}

QString Identity::code() const
{
    return _code;
}

void Identity::prepare(quint64 base) {
    if ( _id == i64UNDEF) {
        _id = base + Identity::_baseId++;
        setName(QString("%1%2").arg(ANONYMOUS_PREFIX).arg(_id)); // default name
    }
}

void Identity::setId(quint64 newid)
{
    _id = newid;
}
