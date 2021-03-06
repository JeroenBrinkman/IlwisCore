#include <QString>
#include "module.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"

using namespace Ilwis;

ColumnDefinition::ColumnDefinition() : _multiple(false)
{
}

ColumnDefinition::ColumnDefinition(const QString &nm, const IDomain &dom, quint64 colindex) : Identity(nm, colindex), _multiple(false)
{
    datadef().domain(dom);
}

bool ColumnDefinition::isValid() const
{
   return name() != sUNDEF && datadef().domain().isValid();
}

QString ColumnDefinition::type() const
{
    return "Column";
}

const DataDefinition &ColumnDefinition::datadef() const
{
    return _datadef;
}

DataDefinition &ColumnDefinition::datadef()
{
    return _datadef;
}

bool ColumnDefinition::isMultiple() const
{
    return _multiple;
}

void ColumnDefinition::multiple(bool yesno)
{
    _multiple = yesno;
}


