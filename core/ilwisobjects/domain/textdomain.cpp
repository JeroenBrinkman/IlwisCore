#include <QUrl>
#include "module.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "mastercatalog.h"
#include "textdomain.h"

using namespace Ilwis;
TextDomain::TextDomain()
{
}

TextDomain::TextDomain(const Resource &resource) : Domain(resource)
{
}

IlwisTypes TextDomain::valueType() const
{
    return itSTRING;
}

QString TextDomain::value(const QVariant &v) const
{
    return v.toString();
}

Domain::Containement TextDomain::contains(const QVariant &) const
{
    return Domain::cSELF;
}

IlwisTypes TextDomain::ilwisType() const
{
    return itTEXTDOMAIN;
}

QSharedPointer<Range> TextDomain::getRange() const
{
    return QSharedPointer<Range>();
}
