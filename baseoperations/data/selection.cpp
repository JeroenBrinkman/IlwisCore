#include "kernel.h"
#include "raster.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "selection.h"

using namespace Ilwis;
using namespace BaseOperations;

Selection::Selection()
{
}


Selection::Selection(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool Selection::execute(ExecutionContext *ctx)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare()) != sPREPARED)
            return false;

    std::function<bool(const Box3D<qint32>)> Assign = [&](const Box3D<qint32> box ) -> bool {
        PixelIterator iterIn(_inputGC, box);
        PixelIterator iterOut(_outputGC, box);
        AttributeRecord rec;
        if ( _attribColumn != "")
            rec = AttributeRecord(_inputGC->attributeTable(itCOVERAGE), "coverage_key");

        double v_in = 0;
        for_each(iterOut, iterOut.end(), [&](double& v){
            v_in = *iterIn;
            if ( v_in != rUNDEF) {
                if ( rec.isValid()) {
                    QVariant var = rec.cellByKey(v,_attribColumn)    ;
                    v = var.toDouble();
                } else {
                    v = v_in;
                }
            }
            ++iterIn;
            ++iterOut;
        });
        return true;
    };

    bool res = OperationHelper::execute(Assign, _outputGC);

    if ( res && ctx != 0) {
        QVariant value;
        value.setValue<IGridCoverage>(_outputGC);

        ctx->_results.push_back(value);
    }
    return res;
}

Ilwis::OperationImplementation *Selection::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Selection(metaid, expr);
}

Ilwis::OperationImplementation::State Selection::prepare()
{
    if ( _expression.parameterCount() != 2) {
        ERROR3(ERR_ILLEGAL_NUM_PARM3,"rasvalue","1",QString::number(_expression.parameterCount()));
        return sPREPAREFAILED;
    }

    QString gc = _expression.parm(0).value();
    if (!_inputGC.prepare(gc)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,gc,"");
        return sPREPAREFAILED;
    }
    OperationHelper helper;
    quint64 copylist = itCOORDSYSTEM;


    QString selector = _expression.parm(1).value();
    int index = selector.indexOf("box=");
    Box2D<double> box;
    if ( index != -1) {
        QString crdlist = "box(" + selector.mid(index+5) + ")";
        _box = Box3D<qint32>(crdlist);
        box = _inputGC->georeference()->pixel2Coord(_box);
        copylist |= itDOMAIN;

    } else {
        QString crdlist = "polygon(" + selector.mid(index+1) + ")";
        _box = Box3D<qint32>(crdlist);
        box = _inputGC->georeference()->pixel2Coord(_box);
        copylist |= itDOMAIN;
    }
    index = selector.indexOf("attribute=");
    if ( index != -1 ) {
        if (! _inputGC->attributeTable(itCOVERAGE).isValid()) {
            ERROR2(ERR_NO_FOUND2,"attribute-table", "coverage");
            return sPREPAREFAILED;
        }
        _attribColumn =  selector.mid(index+11);
        copylist |= itGRIDSIZE | itGEOREF | itENVELOPE;
    }

    helper.initialize(_inputGC, _outputGC, _expression.parm(0), copylist);
    if ( (copylist & itDOMAIN) == 0) {
        if ( _attribColumn != "") {
            _outputGC->datadef() = _inputGC->attributeTable(itCOVERAGE)->columndefinition(_attribColumn).datadef();
        } else {
           _outputGC->datadef() = _inputGC->datadef();
        }
    }
    if ( (copylist & itGEOREF) == 0) {
        Resource res(QUrl("ilwis://internal/georeference"),itCORNERSGEOREF);
        res.addProperty("size", IVARIANT(_box.size()));
        res.addProperty("envelope", IVARIANT(box));
        res.addProperty("coordinatesystem", IVARIANT(_inputGC->coordinateSystem()));
        res.addProperty("name", _outputGC->name());
        res.addProperty("centerofpixel",_inputGC->georeference()->centerOfPixel());
        IGeoReference  grf;
        grf.prepare(res);
        _outputGC->setGeoreference(grf);
    }
    return sPREPARED;
}

quint64 Selection::createMetadata()
{
    QString url = QString("ilwis://operations/selection");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","selection");
    res.addProperty("syntax","selection(coverage,selection-definition)");
    res.addProperty("inparameters","2");
    res.addProperty("pin_1_type", itCOVERAGE);
    res.addProperty("pin_1_name", TR("input coverage"));
    res.addProperty("pin_1_desc",TR("input gridcoverage with a domain as specified by the selection"));
    res.addProperty("pin_2_type", itSTRING);
    res.addProperty("pin_2_name", TR("selection-definition"));
    res.addProperty("pin_2_desc",TR("Selection can either be attribute, layer index or area definition (e.g. box)"));
    res.addProperty("pout_1_type", itCOVERAGE);
    res.addProperty("pout_1_name", TR("coverage were the selection has been applied"));
    res.addProperty("pout_1_desc",TR(""));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();

}

