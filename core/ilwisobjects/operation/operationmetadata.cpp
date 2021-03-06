#include "kernel.h"
#include "ilwisdata.h"
#include "ilwisobject.h"
#include "operationmetadata.h"

using namespace Ilwis;

OperationMetaData::OperationMetaData()
{
}

OperationMetaData::OperationMetaData(const Resource &resource) : IlwisObject(resource)
{
    QString pcount = resource["inparameters"].toString();
    if ( pcount != "") {
        QStringList parts = pcount.split("|");
        parmfromResource(resource,parts.back().toInt(),"pin");
    }
    pcount = resource["outparameters"].toString();
    if ( pcount != "") {
        QStringList parts = pcount.split("|");
        parmfromResource(resource,parts.back().toInt(),"pout");
    }
    _namespace = resource["namespace"].toString();
    _longName = resource["longname"].toString();
}

OperationMetaData::~OperationMetaData()
{
}

void OperationMetaData::parmfromResource(const Resource &resource, int n, const QString& base) {
    for(int i=0; i < n; ++i)    {
        QString parmBase = base + QString("_%1_").arg(i+1);

        bool ok;
        quint64 tp = resource[parmBase + "type"].toLongLong(&ok);
        if (!ok)
            tp = i64UNDEF;
        QString name = resource[parmBase + "name"].toString();
        QString domainName = resource[parmBase + "domain"].toString();
        newParameter(OperationParameter::ptINPUT,name,tp,domainName);
    }
}

IlwisTypes OperationMetaData::ilwisType() const
{
    return itOPERATIONMETADATA;
}

QString OperationMetaData::npace() const
{
    return _namespace;
}


UPOperationParameter& OperationMetaData::newParameter(OperationParameter::ParameterNature kind, const QString &name, IlwisTypes type, const QString &domain)
{
    bool useInput = kind == OperationParameter::ptINPUT;
    for(UPOperationParameter& parm : (useInput ? _inputParameters : _outputParameters)){
        if ( parm->nature() == kind && parm->type() == type)
            return parm;
    }
    if ( useInput)
        _inputParameters.push_back( UPOperationParameter(new OperationParameter(_inputParameters.size(), kind, name, type, domain)));
    else
        _outputParameters.push_back( UPOperationParameter(new OperationParameter(_outputParameters.size(), kind, name, type, domain)));

    return kind == OperationParameter::ptINPUT ? _inputParameters.back() : _outputParameters.back();
}


IlwisTypes OperationParameter::type() const
{
    return _type;
}

OperationParameter::ParameterNature OperationParameter::nature() const
{
    return _nature;
}

quint16 OperationParameter::index() const
{
    return _index;
}

QString OperationParameter::domainName() const
{
    return _domainName;
}

OperationParameter::OperationParameter(quint16 index, OperationParameter::ParameterNature kind, const QString &name, IlwisTypes type, const QString &domain) :
    Identity(name),
    _index(index),
    _nature(kind),
    _type(type),
    _domainName(domain)
{
}
