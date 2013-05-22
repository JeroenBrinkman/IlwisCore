#include <QString>
#include <QVector>
#include <QSqlQuery>
#include <QSqlError>
#include <functional>
#include <future>
#include "kernel.h"
#include "ilwis.h"
#include "angle.h"
#include "point.h"
#include "box.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "domain.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "coordinatesystem.h"
#include "valuedefiner.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "columndefinition.h"
#include "table.h"
#include "containerstatistics.h"
#include "coverage.h"
#include "georeference.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "grid.h"
#include "gridcoverage.h"
#include "gridinterpolator.h"
//#include "identity.h"
#include "OperationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "commandhandler.h"
#include "pixeliterator.h"
#include "blockiterator.h"
#include "operation.h"
#include "operationhelper.h"
#include "resampleraster.h"

using namespace Ilwis;


Ilwis::OperationImplementation *ResampleRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ResampleRaster(metaid, expr);
}

ResampleRaster::ResampleRaster()
{
}

ResampleRaster::ResampleRaster(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr),
    _method(GridCoverage::ipBICUBIC)
{
}

bool ResampleRaster::execute(ExecutionContext *ctx)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare()) != sPREPARED)
            return false;

    auto resampleFun = [&](PixelIterator iterOut) -> bool {
        GridInterpolator interpolator(_inputGC, _method);
        while(iterOut != iterOut.end()) {
           Voxel position = iterOut.position();
           Coordinate c = _outputGC->georeference()->pixel2Coord(position);
           Coordinate c2 = _inputGC->coordinateSystem()->coord2coord(_outputGC->coordinateSystem(),c);
           *iterOut = interpolator.coord2value(c2);
            ++iterOut;
        }
        return true;
    };

    std::vector<Box3D<qint32>> boxes;
    int cores = std::min(QThread::idealThreadCount() - 1, _outputGC->size().ysize());
    if ( _outputGC->size().totalSize() < 10000)
        cores = 1;
    OperationHelper::splitBoxY(cores,_outputGC->size(),boxes);

    std::vector<std::future<bool>> futures(cores);
    bool res = true;

    kernel()->startClock();
    for(int i =0; i < cores; ++i) {
        PixelIterator iter(_outputGC,boxes[i]);
       futures[i] = std::async(std::launch::async, resampleFun, iter);
       // resampleFun(iter);
    }

    for(int i =0; i < cores; ++i) {
        res &= futures[i].get();
    }
    kernel()->endClock();

    if ( res && ctx != 0) {
        QVariant value;
        value.setValue<IGridCoverage>(_outputGC);

        ctx->_results.push_back(value);

        return true;
    }
    return false;
}

Ilwis::OperationImplementation::State ResampleRaster::prepare()
{
    QString gc = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputGC.prepare(gc)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,gc,"");
        return sPREPAREFAILED;
    }
    _box = OperationHelper::initialize(_inputGC, _outputGC, _expression.parm(0),itDOMAIN);
    if ( !_outputGC.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output gridcoverage");
        return sPREPAREFAILED;
    }
    IGeoReference grf;
    grf.prepare(_expression.parm(1).value());
    if ( !grf.isValid()) {
        return sPREPAREFAILED;
    }
    _outputGC->setGeoreference(grf);
    Box2Dd env = grf->pixel2Coord(grf->size());
    _outputGC->setEnvelope(env);
    if ( outputName != sUNDEF)
        _outputGC->setName(outputName);

    QString method = _expression.parm(2).value();
    if ( method.toLower() == "nearestneighbour")
        _method = GridCoverage::ipNEARESTNEIGHBOUR;
    else if ( method.toLower() == "bilinear")
        _method = GridCoverage::ipBILINEAR;
    else if (  method.toLower() == "bicubic")
        _method =GridCoverage::ipBICUBIC;
    else {
        ERROR3(ERR_ILLEGAL_PARM_3,"method",method,"resample");
        return sPREPAREFAILED;
    }

    return sPREPARED;
}

quint64 ResampleRaster::createMetadata()
{
    QString url = QString("ilwis://operations/resample");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","resample");
    res.addProperty("inparameters",3);
    res.addProperty("pin_1_type", itGRIDCOVERAGE);
    res.addProperty("pin_1_name", TR("input gridcoverage"));
    res.addProperty("pin_1_desc",TR("input gridcoverage with domain any domain"));
    res.addProperty("pin_2_type", itGEOREF);
    res.addProperty("pin_2_name", TR("target georeference"));
    res.addProperty("pin_2_desc",TR("the georeference to which the input coverage will be morphed"));
    res.addProperty("pin_3_type", itSTRING);
    res.addProperty("pin_3_name", TR("Resampling method"));
    res.addProperty("pin_3_desc",TR("The method used to aggregate pixels from the input map in the geometry of the output map"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itGRIDCOVERAGE);
    res.addProperty("pout_1_name", TR("output gridcoverage"));
    res.addProperty("pout_1_desc",TR("output gridcoverage with the domain of the input map"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}


