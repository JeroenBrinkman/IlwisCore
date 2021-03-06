#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "binarylogical.h"

using namespace Ilwis;
using namespace BaseOperations;


OperationImplementation *BinaryLogical::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new BinaryLogical( metaid, expr);
}

BinaryLogical::BinaryLogical() : _coveragecoverage(false)
{
}

BinaryLogical::BinaryLogical(quint64 metaid,const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr) , _coveragecoverage(false)
{
}

bool BinaryLogical::setOutput(ExecutionContext *ctx, SymbolTable& symTable) {
    if ( ctx) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputGC);
        ctx->addOutput(symTable,value,_outputGC->name(), itRASTER, _outputGC->source() );
    }
    return _outputGC.isValid();
}

bool BinaryLogical::executeCoverageNumber(ExecutionContext *ctx, SymbolTable& symTable) {

    auto BinaryLogical = [&](const Box3D<qint32> box ) -> bool {
        PixelIterator iterIn(_inputGC1, box);
        PixelIterator iterOut(_outputGC, Box3D<qint32>(box.size()));

        for_each(iterOut, iterOut.end(), [&](double& v){
            double v_in1 = *iterIn;
            if ( v_in1 != rUNDEF && _number != rUNDEF) {
                switch(_operator) {
                case loAND:
                case loEQ:
                    v = v_in1 == _number; break;
                case loOR:
                    v = ((bool)v_in1) || ((bool)_number); break;
                case loXOR:
                    v = ((bool)v_in1) ^ ((bool)_number); break;
                case loLESS:
                    v = v_in1 < _number; break;
                case loLESSEQ:
                    v = v_in1 <= _number; break;
                case loNEQ:
                    v = v_in1 != _number; break;
                case loGREATER:
                    v = v_in1 > _number; break;
                case loGREATEREQ:
                    v = v_in1 >= _number; break;
                default:
                    v = rUNDEF;
                    return false;
                }
            }
            ++iterIn;
        });
        return true;
    };

    if (!OperationHelperRaster::execute(ctx, BinaryLogical, _outputGC))
            return false;


    return setOutput(ctx, symTable);

}

bool BinaryLogical::executeCoverageCoverage(ExecutionContext *ctx, SymbolTable& symTable) {
    std::function<bool(const Box3D<qint32>&)> binaryLogical = [&](const Box3D<qint32>& box ) -> bool {
        PixelIterator iterIn1(_inputGC1, box);
        PixelIterator iterIn2(_inputGC2, box);
        PixelIterator iterOut(_outputGC, Box3D<qint32>(box.size()));

        double v_in1 = 0;
        double v_in2 = 0;
        for_each(iterOut, iterOut.end(), [&](double& v){
            v_in1 = *iterIn1;
            v_in2 = *iterIn2;
            if ( v_in1 != rUNDEF && v_in2 != rUNDEF) {
                switch(_operator) {
                case loAND:
                case loEQ:
                    v = v_in1 == v_in2; break;
                case loOR:
                    v = ((bool)v_in1) || ((bool)v_in2); break;
                case loXOR:
                    v = ((bool)v_in1) ^ ((bool)v_in2); break;
                case loLESS:
                    v = v_in1 < v_in2; break;
                case loLESSEQ:
                    v = v_in1 <= v_in2; break;
                case loNEQ:
                    v = v_in1 != v_in2; break;
                case loGREATER:
                    v = v_in1 > v_in2; break;
                case loGREATEREQ:
                    v = v_in1 >= v_in2; break;
                default:
                    v = rUNDEF;
                    return false;
                }
            }
            ++iterIn1;
            ++iterIn2;
        });
        return true;
    };

   bool resource = OperationHelperRaster::execute(ctx, binaryLogical, _outputGC);

    if (resource && ctx)
        return setOutput(ctx, symTable);

    return false;
}

bool BinaryLogical::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    if ( _coveragecoverage) {
        return executeCoverageCoverage(ctx, symTable);

    } else  {
        return executeCoverageNumber(ctx,symTable);
    }
    return true;
}

bool BinaryLogical::prepareCoverageCoverage() {
    QString raster =  _expression.parm(0).value();
    if (!_inputGC1.prepare(raster)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(raster, ""));
        return false;
    }
    raster =  _expression.parm(1).value();
    if (!_inputGC2.prepare(raster)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(raster, ""));
        return false;
    }
    bool isNumeric = _inputGC1->datadef().domain()->ilwisType() == itNUMERICDOMAIN && _inputGC2->datadef().domain()->ilwisType() == itNUMERICDOMAIN;
    if (!isNumeric ){
        return ERROR2(ERR_INVALID_PROPERTY_FOR_2,"domain","logical operation");
    }

    OperationHelperRaster helper;
    _box = helper.initialize(_inputGC1, _outputGC, _expression.parm(0),
                                itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);

    IDomain dom;
    dom.prepare("bool");
    _outputGC->datadef().domain(dom);
    _coveragecoverage = true;
    return true;
}

bool BinaryLogical::prepareCoverageNumber(IlwisTypes ptype1, IlwisTypes ptype2) {

    int mindex = (ptype1 & itNUMBER) == 0 ? 0 : 1;
    int nindex = mindex ? 0 : 1;

    QString raster =  _expression.parm(mindex).value();
    if (!_inputGC1.prepare(raster)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(raster, ""));
        return false;
    }
    if(_inputGC1->datadef().domain()->ilwisType() != itNUMERICDOMAIN)
        return false;

    _number = _expression.parm(nindex).value().toDouble();

    OperationHelperRaster helper;
    _box = helper.initialize(_inputGC1, _outputGC, _expression.parm(mindex),
                                itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);

     IDomain dom;
    dom.prepare("boolean");
    _outputGC->datadef().domain(dom);

    return true;
}

OperationImplementation::State BinaryLogical::prepare(ExecutionContext *,const SymbolTable&) {
    if ( _expression.parameterCount() != 3){
        return sPREPAREFAILED;
    }

    IlwisTypes ptype1 = _expression.parm(0).valuetype();
    IlwisTypes ptype2 = _expression.parm(1).valuetype();

    QString oper = _expression.parm(2).value();
    oper = oper.toLower();
    if ( oper == "and")
        _operator = loAND;
    else if ( oper == "or")
        _operator = loOR;
    else if ( oper == "xor")
        _operator = loXOR;
    else if ( oper == "less")
        _operator = loLESS;
    else if ( oper == "lesseq")
        _operator = loLESSEQ;
    else if ( oper == "neq")
        _operator = loNEQ;
    else if ( oper == "eq")
        _operator = loEQ;
    else if ( oper == "greater")
        _operator = loGREATER;
    else if ( oper == "greatereq")
        _operator = loGREATEREQ;

    if ( ((ptype1 | ptype2) & (itRASTER | itNUMBER)) ) {
        if(!prepareCoverageNumber(ptype1, ptype2))
            return sPREPAREFAILED;

    } else if ( ptype1 & ptype2 & itRASTER ) {
        if(!prepareCoverageCoverage())
            return sPREPAREFAILED;
    }

    return sPREPARED;
}

quint64 BinaryLogical::createMetadata()
{
    QString url = QString("ilwis://operations/binarylogicalraster");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","binarylogicalraster");
    resource.addProperty("syntax","binarylogicalraster(gridcoverage1,gridcoverage2|number|boolean,and|or|xor|less|lesseq|neq|eq|greater|greatereq)");
        resource.addProperty("description",TR("generates a new boolean map based on the logical condition used"));
    resource.addProperty("inparameters","3");
    resource.addProperty("pin_1_type", itRASTER | itNUMBER);
    resource.addProperty("pin_1_name", TR("input rastercoverage or number/boolean"));
    resource.addProperty("pin_1_domain","value");
    resource.addProperty("pin_1_desc",TR("input rastercoverage with a numerical/boolean domain or number"));
    resource.addProperty("pin_2_type", itRASTER | itNUMBER);
    resource.addProperty("pin_2_name", TR("input rastercoverage or number"));
    resource.addProperty("pin_2_domain","value");
    resource.addProperty("pin_2_desc",TR("input rastercoverage with a numerical/boolean domain or number"));
    resource.addProperty("pin_3_type", itSTRING);
    resource.addProperty("pin_3_name", TR("Operator"));
    resource.addProperty("pin_3_domain","string");
    resource.addProperty("pin_3_desc",TR("operator applied to the other 2 input parameters"));
    resource.addProperty("outparameters",1);
    resource.addProperty("pout_1_type", itRASTER);
    resource.addProperty("pout_1_name", TR("output rastercoverage"));
    resource.addProperty("pout_1_domain","value");
    resource.addProperty("pout_1_desc",TR("output rastercoverage with a boolean domain"));

    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}
