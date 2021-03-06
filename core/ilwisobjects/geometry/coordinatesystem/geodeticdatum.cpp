#include <QSharedPointer>

#include "kernel.h"
#include "module.h"
#include "connectorinterface.h"
#include "identity.h"
#include "angle.h"
#include "point.h"
#include "ilwisdata.h"
#include "geodeticdatum.h"

using namespace Ilwis;

GeodeticDatum::GeodeticDatum(const QString& name) : Identity(name)
{
    _datumParams.resize(10);
    std::fill(_datumParams.begin(),_datumParams.end(), 0.0);
    _datumParams[dmSCALE] = 1;
    _isValid = false;

}

void GeodeticDatum::set3TransformationParameters(double x, double z, double y){
    _datumParams[dmDX] = x;
    _datumParams[dmDY] = y;
    _datumParams[dmDZ] = z;
    setCode(QString("+towgs84=%1,%2,%3").arg(x).arg(y).arg(z));
     _isValid = true;
}

void GeodeticDatum::set7TransformationParameters(double x, double z, double y, double rx, double ry, double rz, double scale){
    set3TransformationParameters(x,y,z);
    _datumParams[dmRX] = rx;
    _datumParams[dmRY] = ry;
    _datumParams[dmRZ] = rz;
    _datumParams[dmSCALE] = scale;
   setCode(QString("%1,%2,%3,%4,%5").arg(code()).arg(x).arg(y).arg(z).arg(scale));


}

void GeodeticDatum::set10TransformationParameters(double x, double z, double y, double rx, double ry, double rz, double scale, Coordinate center){
    set7TransformationParameters(x,y,z,rx,ry,rz,scale);
    _datumParams[dmCENTERXR] = center.x();
    _datumParams[dmCENTERXR] = center.y();
    _datumParams[dmCENTERXR] = center.z();
    setCode(QString("%1,%2,%3,%4").arg(code()).arg( center.x()).arg( center.y()).arg( center.z()));

}

double GeodeticDatum::parameter(DatumParameters parm) const {
    return _datumParams[parm];
}

bool GeodeticDatum::isValid() const
{
    return true; //TODO
}

QString GeodeticDatum::area() const
{
    return _area;
}

void GeodeticDatum::setArea(const QString &v)
{
    _area = v;
}

QString GeodeticDatum::authority() const
{
    return _authority;
}

void GeodeticDatum::setAuthority(const QString& auth){
    _authority = auth;
}






