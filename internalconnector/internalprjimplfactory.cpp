#include <QUrl>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QStringList>

#include "kernel.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "projection.h"
#include "ilwisobjectfactory.h"
#include "projectionimplementation.h"
#include "projectionfactory.h"
#include "internalprjmplfactory.h"
#include "ProjectionImplementation.h"
#include "projections/projectionimplementationinternal.h"
#include "projections/platecaree.h"

using namespace Ilwis;
using namespace Internal;

ProjectionImplFactory::ProjectionImplFactory() : ProjectionFactory("ProjectionFactory","internal")
{
}

ProjectionImplementation *ProjectionImplFactory::create(const Ilwis::Resource &resource) const{
    QString prj = resource.code();

    if ( prj == "PRJPC")
        return new PlateCaree(resource);

    return 0;
}

ProjectionImplementation *ProjectionImplFactory::create(const QString &) const
{
    return nullptr;
}

bool ProjectionImplFactory::canUse(const Ilwis::Resource &resource) const
{
    QString prj = resource.code();
    if ( prj == "PRJPC")
        return true;

    return false;
}

bool ProjectionImplFactory::prepare()
{

    return true;
}
