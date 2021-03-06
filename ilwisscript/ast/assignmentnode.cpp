#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include "kernel.h"
#include "ilwisdata.h"
#include "raster.h"
#include "symboltable.h"
#include "astnode.h"
#include "idnode.h"
#include "operationnode.h"
#include "expressionnode.h"
#include "symboltable.h"
#include "formatter.h"
#include "formatters.h"
#include "scriptnode.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "polygon.h"
#include "geometry.h"
#include "feature.h"
#include "featurecoverage.h"
#include "domain.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "columndefinition.h"
#include "table.h"
#include "connectorinterface.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "ilwiscontext.h"
#include "juliantime.h"
#include "assignmentnode.h"

using namespace Ilwis;

AssignmentNode::AssignmentNode() : _defintion(false)
{
}

bool AssignmentNode::isDefinition() const
{
    return _defintion;
}

void AssignmentNode::setDefintion(bool yesno)
{
    _defintion= yesno;
}

void AssignmentNode::setResult(IDNode *node)
{
    _result =QSharedPointer<IDNode>(node);
}

void AssignmentNode::setFormatPart(ASTNode *node)
{
    _typemodifier = QSharedPointer<ASTNode>(node);
}

void AssignmentNode::setExpression(ExpressionNode *node)
{
    _expression =QSharedPointer<ExpressionNode>(node);
}

QString AssignmentNode::nodeType() const
{
    return "assignment";
}

IIlwisObject AssignmentNode::getObject(const Symbol& sym) const {
    IlwisTypes tp = sym._type;
    if ( tp & itRASTER)
            return sym._var.value<Ilwis::IRasterCoverage>().get<IlwisObject>();
    if ( tp & itFEATURE)
            return sym._var.value<Ilwis::IFeatureCoverage>().get<IlwisObject>();
    return IIlwisObject();

}

void AssignmentNode::getFormat(ASTNode *node, QString& format, QString& fnamespace) const {
    Formatter *fnode = static_cast<Formatter *>(node->child(0).data());
    format = fnode->format();
    fnamespace = fnode->fnamespace();

    if ( format == "" || format == sUNDEF) {
        Formatter *fnode = ScriptNode::activeFormat(itRASTER);
        if ( fnode) {
            format = fnode->format();
            fnamespace = fnode->fnamespace();
        }
    }
}

void AssignmentNode::store2Format(ASTNode *node, const Symbol& sym, const QString& result) {
    QString format, fnamespace;
    getFormat(node, format, fnamespace);
    if ( format != "" && format != sUNDEF) {
        Ilwis::IIlwisObject object = getObject(sym);
        bool wasAnonymous = object->isAnonymous();
        object->setName(result);
        object->connectTo(QUrl(), format, fnamespace, Ilwis::IlwisObject::cmOUTPUT);
        object->setCreateTime(Ilwis::Time::now());
        if ( wasAnonymous)
            mastercatalog()->addItems({object->source(IlwisObject::cmOUTPUT | IlwisObject::cmEXTENDED)});

        object->store(Ilwis::IlwisObject::smMETADATA | Ilwis::IlwisObject::smBINARYDATA);

     }
}

bool AssignmentNode::evaluate(SymbolTable& symbols, int scope, ExecutionContext *ctx)
{
    if ( _expression.isNull())
        return false;


    bool res = _expression->evaluate(symbols, scope, ctx);
    if ( res) {
        NodeValue val = _expression->value();
        Symbol sym = symbols.getSymbol(val.toString(),SymbolTable::gaREMOVEIFANON);
        IlwisTypes tp = sym.isValid() ? sym._type : itUNKNOWN;
        QString result = _result->id();
        if ( !_typemodifier.isNull()) {
            ASTNode *node = static_cast<ASTNode *>(_typemodifier.data());
            if ( node->noOfChilderen()!= 1)
                return ERROR2(ERR_NO_OBJECT_TYPE_FOR_2, "Output object", "expression");
            store2Format(node, sym, result);

        }
        if (  tp & itCOVERAGE) {
            bool ok;
            if ( tp & itRASTER) {
                ok = copyObject<RasterCoverage>(sym, result,symbols);
            }
            else
                ok = copyObject<FeatureCoverage>(sym, result,symbols);
            if(!ok) {
                throw ErrorObject(QString(TR(ERR_OPERATION_FAILID1).arg("assignment")));
            }

            return ok;

        } else {
            sym = symbols.getSymbol(_result->id(),SymbolTable::gaREMOVEIFANON);
            tp = sym.isValid() ? sym._type : itUNKNOWN;
            if ( tp == itUNKNOWN) {
                tp = Domain::ilwType(val);
            }
        }
        symbols.addSymbol(_result->id(), scope, tp, _expression->value());
        //symbols.addSymbol(_result->id(), scope, tp, sym._var);

        return true;
    }
    return false;
}

